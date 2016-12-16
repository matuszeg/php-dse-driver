/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/bytes.h"
#include "util/collections.h"
#include "util/consistency.h"
#include "util/future.h"
#include "util/hash.h"
#include "util/ref.h"
#include "util/result.h"
#include "util/math.h"

#include "Graph/ResultSet.h"
#include "LineString.h"
#include "Point.h"
#include "Polygon.h"

#include <ext/standard/base64.h>

zend_class_entry *php_driver_default_session_ce = NULL;

#define CHECK_RESULT(rc) \
{ \
  ASSERT_SUCCESS_VALUE(rc, FAILURE) \
  return SUCCESS; \
}

static void
free_result(void *result)
{
  cass_result_free((CassResult *) result);
}

static void
free_statement(void *statement)
{
  cass_statement_free((CassStatement *) statement);
}

static void
free_schema(void *schema)
{
  cass_schema_meta_free((CassSchemaMeta *) schema);
}

static int
bind_argument_by_index(CassStatement *statement, size_t index, zval *value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL)
    CHECK_RESULT(cass_statement_bind_null(statement, index));

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string(statement, index, Z_STRVAL_P(value)));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double(statement, index, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32(statement, index, Z_LVAL_P(value)));

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(cass_statement_bind_bool(statement, index, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(cass_statement_bind_bool(statement, index, cass_false));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_float(statement, index, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int16(statement, index, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int8(statement, index, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(cass_statement_bind_uint32(statement, index, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      CHECK_RESULT(cass_statement_bind_bytes(statement, index, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = export_twos_complement(varint->varint_value, &size);
      CassError rc = cass_statement_bind_bytes(statement, index, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(decimal->decimal_value, &size);
      CassError rc = cass_statement_bind_decimal(statement, index, data, size, decimal->decimal_scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      CHECK_RESULT(cass_statement_bind_uuid(statement, index, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      CHECK_RESULT(cass_statement_bind_inet(statement, index, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (!php_driver_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (!php_driver_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (!php_driver_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      CassError rc;
      CassTuple *tup;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (!php_driver_tuple_from_tuple(tuple, &tup TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_tuple(statement, index, tup);
      cass_tuple_free(tup);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      CassError rc;
      CassUserType *ut;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (!php_driver_user_type_from_user_type_value(user_type_value, &ut TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_user_type(statement, index, ut);
      cass_user_type_free(ut);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_line_string_ce TSRMLS_CC)) {
      return php_driver_line_string_bind_by_index(statement, index, value TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_point_ce TSRMLS_CC)) {
      return php_driver_point_bind_by_index(statement, index, value TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_polygon_ce TSRMLS_CC)) {
      return php_driver_polygon_bind_by_index(statement, index, value TSRMLS_CC);
    }
  }

  return FAILURE;
}

static int
bind_argument_by_name(CassStatement *statement, const char *name,
                      zval *value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(cass_statement_bind_null_by_name(statement, name));
  }

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string_by_name(statement, name, Z_STRVAL_P(value)));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double_by_name(statement, name, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32_by_name(statement, name, Z_LVAL_P(value)));

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, name, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, name, cass_false));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_float_by_name(statement, name, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int16_by_name(statement, name, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int8_by_name(statement, name, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(cass_statement_bind_uint32_by_name(statement, name, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, name, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(varint->varint_value, &size);
      CassError rc = cass_statement_bind_bytes_by_name(statement, name, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(decimal->decimal_value, &size);
      CassError rc = cass_statement_bind_decimal_by_name(statement, name, data, size, decimal->decimal_scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, name, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      CHECK_RESULT(cass_statement_bind_inet_by_name(statement, name, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (!php_driver_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (!php_driver_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (!php_driver_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      CassError rc;
      CassTuple *tup;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (!php_driver_tuple_from_tuple(tuple, &tup TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_tuple_by_name(statement, name, tup);
      cass_tuple_free(tup);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      CassError rc;
      CassUserType *ut;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (!php_driver_user_type_from_user_type_value(user_type_value, &ut TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_user_type_by_name(statement, name, ut);
      cass_user_type_free(ut);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_line_string_ce TSRMLS_CC)) {
      return php_driver_line_string_bind_by_name(statement, name, value TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_point_ce TSRMLS_CC)) {
      return php_driver_point_bind_by_name(statement, name, value TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_polygon_ce TSRMLS_CC)) {
      return php_driver_polygon_bind_by_name(statement, name, value TSRMLS_CC);
    }
  }

  return FAILURE;
}

static int
bind_arguments(CassStatement *statement, HashTable *arguments TSRMLS_DC)
{
  int rc = SUCCESS;

  php5to7_zval *current;
  ulong num_key;

#if PHP_MAJOR_VERSION >= 7
  zend_string *key;
  ZEND_HASH_FOREACH_KEY_VAL(arguments, num_key, key, current) {
    if (key) {
      rc = bind_argument_by_name(statement, key->val,
                                 PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#else
  char *str_key;
  uint str_len;
  PHP5TO7_ZEND_HASH_FOREACH_KEY_VAL(arguments, num_key, str_key, str_len, current) {
    if (str_key) {
      rc = bind_argument_by_name(statement, str_key,
                                 PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#endif
    } else {
      rc = bind_argument_by_index(statement, num_key, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
    }
    if (rc == FAILURE) break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(arguments);

  return rc;
}

static CassStatement *
create_statement(php_driver_statement *statement, HashTable *arguments TSRMLS_DC)
{
  CassStatement *stmt;
  uint32_t count;

  switch (statement->type) {
  case PHP_DRIVER_SIMPLE_STATEMENT:
    count  = 0;

    if (arguments)
      count = zend_hash_num_elements(arguments);

    stmt = cass_statement_new(statement->cql, count);
    break;
  case PHP_DRIVER_PREPARED_STATEMENT:
    stmt = cass_prepared_bind(statement->prepared);
    break;
  default:
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
      "Unsupported statement type.");
    return NULL;
  }

  if (arguments && bind_arguments(stmt, arguments TSRMLS_CC) == FAILURE) {
    cass_statement_free(stmt);
    return NULL;
  }

  return stmt;
}

static CassBatch *
create_batch(php_driver_statement *batch,
             CassConsistency consistency,
             CassRetryPolicy *retry_policy,
             cass_int64_t timestamp TSRMLS_DC)
{
  CassBatch *cass_batch = cass_batch_new(batch->batch_type);
  CassError rc = CASS_OK;

  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(&batch->statements, current) {
#if PHP_MAJOR_VERSION >= 7
    php_driver_batch_statement_entry *batch_statement_entry = (php_driver_batch_statement_entry *)Z_PTR_P(current);
#else
    php_driver_batch_statement_entry *batch_statement_entry = *((php_driver_batch_statement_entry **)current);
#endif
    php_driver_statement *statement =
        PHP_DRIVER_GET_STATEMENT(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->statement));
    HashTable *arguments
        = !PHP5TO7_ZVAL_IS_UNDEF(batch_statement_entry->arguments)
          ? Z_ARRVAL_P(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->arguments))
          : NULL;
    CassStatement *stmt = create_statement(statement, arguments TSRMLS_CC);
    if (!stmt) {
      cass_batch_free(cass_batch);
      return NULL;
    }
    cass_batch_add_statement(cass_batch, stmt);
    cass_statement_free(stmt);
  } PHP5TO7_ZEND_HASH_FOREACH_END(&batch->statements);

  rc = cass_batch_set_consistency(cass_batch, consistency);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  rc = cass_batch_set_retry_policy(cass_batch, retry_policy);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  rc = cass_batch_set_timestamp(cass_batch, timestamp);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  return cass_batch;
}

static CassStatement *
create_single(php_driver_statement *statement, HashTable *arguments,
              CassConsistency consistency, long serial_consistency,
              int page_size, const char* paging_state_token,
              size_t paging_state_token_size,
              CassRetryPolicy *retry_policy, cass_int64_t timestamp TSRMLS_DC)
{
  CassError rc = CASS_OK;
  CassStatement *stmt = create_statement(statement, arguments TSRMLS_CC);
  if (!stmt)
    return NULL;

  rc = cass_statement_set_consistency(stmt, consistency);

  if (rc == CASS_OK && serial_consistency >= 0)
    rc = cass_statement_set_serial_consistency(stmt, serial_consistency);

  if (rc == CASS_OK && page_size >= 0)
    rc = cass_statement_set_paging_size(stmt, page_size);

  if (rc == CASS_OK && paging_state_token) {
    rc = cass_statement_set_paging_state_token(stmt,
                                               paging_state_token,
                                               paging_state_token_size);
  }

  if (rc == CASS_OK && retry_policy)
    rc = cass_statement_set_retry_policy(stmt, retry_policy);

  if (rc == CASS_OK)
    rc = cass_statement_set_timestamp(stmt, timestamp);

  if (rc != CASS_OK) {
    cass_statement_free(stmt);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
                            "%s", cass_error_desc(rc));
    return NULL;
  }

  return stmt;
}

static int
graph_array_from_set(php_driver_set *set,
                     DseGraphArray **result TSRMLS_DC);

static int
graph_array_from_collection(php_driver_collection *coll,
                            DseGraphArray **result TSRMLS_DC);

static int
graph_object_from_map(php_driver_map *map,
                      DseGraphObject **result TSRMLS_DC);

static int
graph_array_from_tuple(php_driver_tuple *tuple,
                       DseGraphArray **result TSRMLS_DC);

static int
graph_object_from_user_type_value(php_driver_user_type_value *user_type_value,
                                  DseGraphObject **result TSRMLS_DC);

static int
graph_build_array(zval *value,
                  DseGraphArray **result TSRMLS_DC);

static int
graph_build_object(zval *value,
                   DseGraphObject **result TSRMLS_DC);

static int
graph_object_add_uuid(DseGraphObject *object,
                      const char *name,
                      php_driver_uuid *uuid TSRMLS_DC)
{
  char uuid_str[CASS_UUID_STRING_LENGTH];
  cass_uuid_string(uuid->uuid, uuid_str);
  CHECK_RESULT(dse_graph_object_add_string(object, name, uuid_str));
}

static int
graph_array_add_uuid(DseGraphArray *array,
                     php_driver_uuid *uuid TSRMLS_DC)
{
  char uuid_str[CASS_UUID_STRING_LENGTH];
  cass_uuid_string(uuid->uuid, uuid_str);
  CHECK_RESULT(dse_graph_array_add_string(array, uuid_str));
}

static int
graph_object_add_inet(DseGraphObject *object,
                      const char *name,
                      php_driver_inet *inet TSRMLS_DC)
{
  char inet_str[CASS_INET_STRING_LENGTH];
  cass_inet_string(inet->inet, inet_str);
  CHECK_RESULT(dse_graph_object_add_string(object, name, inet_str));
}

static int
graph_array_add_inet(DseGraphArray *array,
                     php_driver_inet *inet TSRMLS_DC)
{
  char inet_str[CASS_INET_STRING_LENGTH];
  cass_inet_string(inet->inet, inet_str);
  CHECK_RESULT(dse_graph_array_add_string(array, inet_str));
}

static int
graph_object_add_varint(DseGraphObject *object,
                        const char *name,
                        php_driver_numeric *varint TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_driver_format_integer(varint->varint_value, &str, &len);
  rc = dse_graph_object_add_string_n(object,
                                     name, strlen(name),
                                     str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int
graph_array_add_varint(DseGraphArray *array,
                       php_driver_numeric *varint TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_driver_format_integer(varint->varint_value, &str, &len);
  rc = dse_graph_array_add_string_n(array, str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int
graph_object_add_decimal(DseGraphObject *object,
                         const char *name,
                         php_driver_numeric *decimal TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_driver_format_decimal(decimal->decimal_value, decimal->decimal_scale,
                               &str, &len);
  rc = dse_graph_object_add_string_n(object,
                                     name, strlen(name),
                                     str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int
graph_array_add_decimal(DseGraphArray *array,
                        php_driver_numeric *decimal TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_driver_format_decimal(decimal->decimal_value, decimal->decimal_scale,
                               &str, &len);
  rc = dse_graph_array_add_string_n(array, str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int
graph_object_add_blob(DseGraphObject *object,
                      const char *name,
                      php_driver_blob *blob TSRMLS_DC)
{
#if PHP_MAJOR_VERSION >= 7
  zend_string *data = php_base64_encode(blob->data, blob->size);
  CassError rc = dse_graph_object_add_string_n(object,
                                               name, strlen(name),
                                               data->val, data->len);
  zend_string_release(data);
#else
  int size;
  unsigned char *data = php_base64_encode(blob->data, blob->size, &size);
  CassError rc = dse_graph_object_add_string_n(object,
                                               name, strlen(name),
                                               (char *)data, size);
  efree(data);
#endif

  CHECK_RESULT(rc);
}

static int
graph_array_add_blob(DseGraphArray *array,
                     php_driver_blob *blob TSRMLS_DC)
{
#if PHP_MAJOR_VERSION >= 7
  zend_string *data = php_base64_encode(blob->data, blob->size);
  CassError rc = dse_graph_array_add_string_n(array, data->val, data->len);
  zend_string_release(data);
#else
  int size;
  unsigned char *data = php_base64_encode(blob->data, blob->size, &size);
  CassError rc = dse_graph_array_add_string_n(array, (char *)data, size);
  efree(data);
#endif

  CHECK_RESULT(rc);
}

static int
graph_object_add_with_value_type(DseGraphObject *object,
                                 const char *name,
                                 zval *value,
                                 CassValueType type TSRMLS_DC)
{
  CassError             rc;
  php_driver_blob       *blob;
  php_driver_numeric    *numeric;
  php_driver_timestamp  *timestamp;
  php_driver_date       *date;
  php_driver_time       *time;
  php_driver_uuid       *uuid;
  php_driver_inet       *inet;
  php_driver_collection *coll;
  php_driver_map        *map;
  php_driver_set        *set;
  php_driver_tuple      *tuple;
  php_driver_user_type_value *user_type_value;
  DseGraphObject       *sub_object;
  DseGraphArray        *sub_array;

  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(dse_graph_object_add_null(object, name));
  }

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_RESULT(dse_graph_object_add_string_n(object,
                                               name, strlen(name),
                                               Z_STRVAL_P(value), Z_STRLEN_P(value)));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_SMALL_INT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int32(object, name, numeric->smallint_value));
    break;
  case CASS_VALUE_TYPE_TINY_INT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int32(object, name, numeric->tinyint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_DRIVER_GET_BLOB(value);
    CHECK_RESULT(graph_object_add_blob(object, name, blob TSRMLS_CC));
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
#if PHP_MAJOR_VERSION >= 7
    CHECK_RESULT(dse_graph_object_add_bool(object, name, Z_TYPE_P(value) == IS_TRUE ? cass_true : cass_false));
#else
    CHECK_RESULT(dse_graph_object_add_bool(object, name, Z_BVAL_P(value)));
#endif
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    CHECK_RESULT(dse_graph_object_add_double(object, name, Z_DVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_FLOAT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_double(object, name, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_RESULT(dse_graph_object_add_int32(object, name, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_DATE:
    date = PHP_DRIVER_GET_DATE(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, date->date));
    break;
  case CASS_VALUE_TYPE_TIME:
    time = PHP_DRIVER_GET_TIME(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, time->time));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_DRIVER_GET_UUID(value);
    return graph_object_add_uuid(object, name, uuid TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    return graph_object_add_varint(object, name, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    return graph_object_add_decimal(object, name, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_DRIVER_GET_INET(value);
    return graph_object_add_inet(object, name, inet TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_DRIVER_GET_COLLECTION(value);
    if (graph_array_from_collection(coll, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_DRIVER_GET_MAP(value);
    if (graph_object_from_map(map, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_object(object, name, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_DRIVER_GET_SET(value);
    if (graph_array_from_set(set, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_TUPLE:
    tuple = PHP_DRIVER_GET_TUPLE(value);
    if (graph_array_from_tuple(tuple, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
    if (graph_object_from_user_type_value(user_type_value, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_object(object, name, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  default:
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return FAILURE;
  }

  return FAILURE;
}

static int
graph_array_add_with_value_type(DseGraphArray *array,
                                zval *value,
                                CassValueType type TSRMLS_DC)
{
  CassError             rc;
  php_driver_blob       *blob;
  php_driver_numeric    *numeric;
  php_driver_timestamp  *timestamp;
  php_driver_date       *date;
  php_driver_time       *time;
  php_driver_uuid       *uuid;
  php_driver_inet       *inet;
  php_driver_collection *coll;
  php_driver_map        *map;
  php_driver_set        *set;
  php_driver_tuple      *tuple;
  php_driver_user_type_value *user_type_value;
  DseGraphObject       *sub_object;
  DseGraphArray        *sub_array;

  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(dse_graph_array_add_null(array));
  }

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_RESULT(dse_graph_array_add_string_n(array, Z_STRVAL_P(value), Z_STRLEN_P(value)));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_SMALL_INT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int32(array, numeric->smallint_value));
    break;
  case CASS_VALUE_TYPE_TINY_INT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int32(array, numeric->tinyint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_DRIVER_GET_BLOB(value);
    CHECK_RESULT(graph_array_add_blob(array, blob TSRMLS_CC));
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
#if PHP_MAJOR_VERSION >= 7
    CHECK_RESULT(dse_graph_array_add_bool(array, Z_TYPE_P(value) == IS_TRUE ? cass_true : cass_false));
#else
    CHECK_RESULT(dse_graph_array_add_bool(array, Z_BVAL_P(value)));
#endif
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    CHECK_RESULT(dse_graph_array_add_double(array, Z_DVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_FLOAT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_double(array, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_RESULT(dse_graph_array_add_int32(array, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_DATE:
    date = PHP_DRIVER_GET_DATE(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, date->date));
    break;
  case CASS_VALUE_TYPE_TIME:
    time = PHP_DRIVER_GET_TIME(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, time->time));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_DRIVER_GET_UUID(value);
    return graph_array_add_uuid(array, uuid TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    return graph_array_add_varint(array, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_DRIVER_GET_NUMERIC(value);
    return graph_array_add_decimal(array, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_DRIVER_GET_INET(value);
    return graph_array_add_inet(array, inet TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_DRIVER_GET_COLLECTION(value);
    if (graph_array_from_collection(coll, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_DRIVER_GET_MAP(value);
    if (graph_object_from_map(map, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_object(array, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_DRIVER_GET_SET(value);
    if (graph_array_from_set(set, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_TUPLE:
    tuple = PHP_DRIVER_GET_TUPLE(value);
    if (graph_array_from_tuple(tuple, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
    if (graph_object_from_user_type_value(user_type_value, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_object(array, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  default:
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return FAILURE;
  }

  return FAILURE;
}

static int
graph_array_from_set(php_driver_set *set,
                     DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  php_driver_type *type;
  php_driver_type *value_type;
  DseGraphArray *array;
  php_driver_set_entry *curr, *temp;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));
  value_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
  array = dse_graph_array_new();

  HASH_ITER(hh, set->entries, curr, temp) {
    if (graph_array_add_with_value_type(array,
                                        PHP5TO7_ZVAL_MAYBE_P(curr->value),
                                        value_type->type TSRMLS_CC) == FAILURE) {
      rc = FAILURE;
      break;
    }
  }

  if (rc == SUCCESS)
    *result = array;
  else
    dse_graph_array_free(array);

  return rc;
}

static int
graph_array_from_collection(php_driver_collection *coll,
                            DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  php_driver_type *type;
  php_driver_type *value_type;
  DseGraphArray *array;
  php5to7_zval *curr;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(coll->type));
  value_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
  array = dse_graph_array_new();

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&coll->values, curr) {
    if (graph_array_add_with_value_type(array,
                                        PHP5TO7_ZVAL_MAYBE_DEREF(curr),
                                        value_type->type TSRMLS_CC) == FAILURE) {
      rc = FAILURE;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&coll->values);


  if (rc == SUCCESS)
    *result = array;
  else
    dse_graph_array_free(array);

  return rc;
}

static int
graph_object_from_map(php_driver_map *map,
                      DseGraphObject **result TSRMLS_DC)
{
  int rc = SUCCESS;
  php_driver_type *type;
  php_driver_type *key_type;
  php_driver_type *value_type;
  DseGraphObject *object;
  php_driver_map_entry *curr, *temp;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));
  value_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
  key_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->key_type));
  object = dse_graph_object_new();

  HASH_ITER(hh, map->entries, curr, temp) {
    if (graph_object_add_with_value_type(object,
                                         PHP5TO7_Z_STRVAL_MAYBE_P(curr->key), /* TODO: Fix this? Should this do a string conversion? */
                                         PHP5TO7_ZVAL_MAYBE_P(curr->value),
                                         value_type->type TSRMLS_CC) == FAILURE) {
      rc = FAILURE;
      break;
    }
  }

  if (rc == SUCCESS)
    *result = object;
  else
    dse_graph_object_free(object);

  return rc;
}

static int
graph_array_from_tuple(php_driver_tuple *tuple,
                       DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  php5to7_ulong num_key;
  php5to7_zval *current;
  php_driver_type *type;
  DseGraphArray *array;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(tuple->type));
  array = dse_graph_array_new();

  PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(&tuple->values, num_key, current) {
    php5to7_zval *zsub_type;
    php_driver_type *sub_type;
    PHP5TO7_ZEND_HASH_INDEX_FIND(&type->types, num_key, zsub_type);
    if (!php_driver_validate_object(PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                       PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type) TSRMLS_CC)) {
      rc = FAILURE;
      break;
    }
    sub_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type));
    if (graph_array_add_with_value_type(array,
                                        PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                        sub_type->type TSRMLS_CC) == FAILURE) {
      rc = FAILURE;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&tuple->values);

  if (rc == SUCCESS)
    *result = array;
  else
    dse_graph_array_free(array);

  return rc;
}

static int
graph_object_from_user_type_value(php_driver_user_type_value *user_type_value,
                                  DseGraphObject **result TSRMLS_DC)
{
  int rc = SUCCESS;
  char *name;
  php5to7_zval *current;
  php_driver_type *type;
  DseGraphObject *object;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value->type));
  object = dse_graph_object_new();

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(&user_type_value->values, name, current) {
    php5to7_zval *zsub_type;
    php_driver_type *sub_type;
    if (!PHP5TO7_ZEND_HASH_FIND(&type->types, name, strlen(name) + 1, zsub_type) ||
        !php_driver_validate_object(PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                       PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type) TSRMLS_CC)) {
      rc = FAILURE;
      break;
    }
    sub_type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type));
    if (graph_object_add_with_value_type(object, name,
                                         PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                         sub_type->type TSRMLS_CC) == FAILURE) {
      rc = FAILURE;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&user_type_value->values);

  if (rc)
    *result = object;
  else
    dse_graph_object_free(object);

  return rc;
}

#define GRAPH_OUTPUT_ARRAY  0
#define GRAPH_OUTPUT_OBJECT 1

static int
graph_determine_array_type(zval *value) {
  HashTable *ht = Z_ARRVAL_P(value);

  if (ht && zend_hash_num_elements(ht) > 0) {
    php5to7_zval *current;
    ulong num_key, index = 0;

#if PHP_MAJOR_VERSION >= 7
    zend_string *key;
    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, current) {
      if (key) {
#else
    char *str_key;
    uint str_len;
    PHP5TO7_ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, str_len, current) {
      if (str_key) {
#endif
        return GRAPH_OUTPUT_OBJECT;
      } else if (index != num_key) {
        return GRAPH_OUTPUT_OBJECT;
      }
      index++;
    } PHP5TO7_ZEND_HASH_FOREACH_END(ht);

  }

  return GRAPH_OUTPUT_ARRAY;
}

static int
graph_array_add(DseGraphArray *array,
                zval *value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(dse_graph_array_add_null(array));
  }

  if (Z_TYPE_P(value) == IS_STRING) {
    CHECK_RESULT(dse_graph_array_add_string(array, Z_STRVAL_P(value)));
  }

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(dse_graph_array_add_double(array, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(dse_graph_array_add_int32(array, Z_LVAL_P(value)));

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(dse_graph_array_add_bool(array, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(dse_graph_array_add_bool(array, cass_false));

  if (Z_TYPE_P(value) == IS_ARRAY) {
    if (graph_determine_array_type(value) == GRAPH_OUTPUT_ARRAY) {
      DseGraphArray *sub_array;
      if (graph_build_array(value, &sub_array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      CHECK_RESULT(dse_graph_array_add_array(array, sub_array));
    } else {
      DseGraphObject *sub_object;
      if (graph_build_object(value, &sub_object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      CHECK_RESULT(dse_graph_array_add_object(array, sub_object));
    }
  }

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_double(array, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int32(array, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int32(array, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      return graph_array_add_blob(array, blob TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      return graph_array_add_varint(array, varint TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      return graph_array_add_decimal(array, decimal TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      return graph_array_add_uuid(array, uuid TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      return graph_array_add_inet(array, inet TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (graph_array_from_set(set, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (graph_object_from_map(map, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_object(array, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (graph_array_from_collection(coll, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (graph_array_from_tuple(tuple, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (graph_object_from_user_type_value(user_type_value, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_object(array, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_line_string_ce TSRMLS_CC)) {
      char* wkt = php_driver_line_string_to_wkt(PHP_DRIVER_GET_LINE_STRING(value) TSRMLS_CC);
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_point_ce TSRMLS_CC)) {
      char* wkt = php_driver_point_to_wkt(PHP_DRIVER_GET_POINT(value));
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_polygon_ce TSRMLS_CC)) {
      char* wkt = php_driver_polygon_to_wkt(PHP_DRIVER_GET_POLYGON(value) TSRMLS_CC);
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
graph_object_add(DseGraphObject *object,
                 const char *name,
                 zval *value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(dse_graph_object_add_null(object, name));
  }

  if (Z_TYPE_P(value) == IS_STRING) {
    CHECK_RESULT(dse_graph_object_add_string(object, name, Z_STRVAL_P(value)));
  }

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(dse_graph_object_add_double(object, name, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(dse_graph_object_add_int32(object, name, Z_LVAL_P(value)));

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(dse_graph_object_add_bool(object, name, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(dse_graph_object_add_bool(object, name, cass_false));

  if (Z_TYPE_P(value) == IS_ARRAY) {
    if (graph_determine_array_type(value) == GRAPH_OUTPUT_ARRAY) {
      DseGraphArray *sub_array;
      if (graph_build_array(value, &sub_array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      CHECK_RESULT(dse_graph_object_add_array(object, name, sub_array));
    } else {
      DseGraphObject *sub_object;
      if (graph_build_object(value, &sub_object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      CHECK_RESULT(dse_graph_object_add_object(object, name, sub_object));
    }
  }

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_double(object, name, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int32(object, name, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int32(object, name, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      return graph_object_add_blob(object, name, blob TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      return graph_object_add_varint(object, name, varint TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      return graph_object_add_decimal(object, name, decimal TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      return graph_object_add_uuid(object, name, uuid TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      return graph_object_add_inet(object, name, inet TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (graph_array_from_set(set, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (graph_object_from_map(map, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_object(object, name, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (graph_array_from_collection(coll, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (graph_array_from_tuple(tuple, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (graph_object_from_user_type_value(user_type_value, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_object(object, name, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_line_string_ce TSRMLS_CC)) {
      char* wkt = php_driver_line_string_to_wkt(PHP_DRIVER_GET_LINE_STRING(value) TSRMLS_CC);
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_point_ce TSRMLS_CC)) {
      char* wkt = php_driver_point_to_wkt(PHP_DRIVER_GET_POINT(value));
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_polygon_ce TSRMLS_CC)) {
      char* wkt = php_driver_polygon_to_wkt(PHP_DRIVER_GET_POLYGON(value) TSRMLS_CC);
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
graph_build_array(zval *value,
                  DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;

  HashTable *ht = Z_ARRVAL_P(value);
  DseGraphArray *array = dse_graph_array_new();

  php5to7_ulong num_key;
  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(ht, num_key, current) {
    CassError r = graph_array_add(array, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(r, rc = FAILURE; break;);
  } PHP5TO7_ZEND_HASH_FOREACH_END(ht);

  if (rc == SUCCESS) {
    dse_graph_array_finish(array);
    *result = array;
  } else {
    dse_graph_array_free(array);
  }

  return rc;
}

static int
graph_build_object(zval *value,
                   DseGraphObject **result TSRMLS_DC)
{
  int rc = SUCCESS;

  HashTable *ht = Z_ARRVAL_P(value);
  DseGraphObject *object = dse_graph_object_new();

  if (ht && zend_hash_num_elements(ht) > 0) {
    php5to7_zval *current;
    ulong num_key;
    CassError r = CASS_OK;

#if PHP_MAJOR_VERSION >= 7
    zend_string *key;
    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, key, current) {
      if (key) {
        r = graph_object_add(object, key->val, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#else
    char *str_key;
    uint str_len;
    PHP5TO7_ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, str_len, current) {
      if (str_key) {
        r = graph_object_add(object, str_key, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#endif
      } else {
        char num_key_str[64];
        sprintf(num_key_str, "%lu", num_key);
        r = graph_object_add(object, num_key_str, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
      }

      ASSERT_SUCCESS_BLOCK(r, rc = FAILURE; break;)
    } PHP5TO7_ZEND_HASH_FOREACH_END(ht);
  }

  if (rc == SUCCESS) {
    dse_graph_object_finish(object);
    *result = object;
  } else {
    dse_graph_object_free(object);
  }

  return rc;
}

static DseGraphObject *
build_graph_arguments(php5to7_zval *arguments TSRMLS_DC)
{
  char *name;
  php5to7_zval *current;
  DseGraphObject *result = dse_graph_object_new();

  HashTable *array = PHP5TO7_Z_ARRVAL_MAYBE_P(*arguments);

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(array, name, current) {
    if (!name) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "arguments must be an array of key/value pairs");
      dse_graph_object_free(result);
      return NULL;
    }
    if (graph_object_add(result, name, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC) == FAILURE) {
      dse_graph_object_free(result);
      return NULL;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(array);

  dse_graph_object_finish(result);

  return result;
}

static DseGraphOptions *
build_graph_options(php_driver_session *session,
                    zval *options TSRMLS_DC)
{
  php5to7_zval *value;
  DseGraphOptions *graph_options;

  if (!PHP5TO7_ZVAL_IS_UNDEF(session->graph_options)) {
    graph_options = dse_graph_options_new_from_existing(
                      PHP_DRIVER_GET_GRAPH_OPTIONS(
                        PHP5TO7_ZVAL_MAYBE_P(session->graph_options))->options);

  } else {
    graph_options = dse_graph_options_new();
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "graph_language", sizeof("graph_language"), value)) {
    if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) != IS_STRING) {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "graph_language", "a string" TSRMLS_CC);
      dse_graph_options_free(graph_options);
      return NULL;
    }
    dse_graph_options_set_graph_language_n(graph_options,
                                           PHP5TO7_Z_STRVAL_MAYBE_P(*value),
                                           PHP5TO7_Z_STRLEN_MAYBE_P(*value));
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "graph_source", sizeof("graph_source"), value)) {
    if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) != IS_STRING) {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "graph_source", "a string" TSRMLS_CC);
      dse_graph_options_free(graph_options);
      return NULL;
    }
    dse_graph_options_set_graph_source_n(graph_options,
                                         PHP5TO7_Z_STRVAL_MAYBE_P(*value),
                                         PHP5TO7_Z_STRLEN_MAYBE_P(*value));
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "graph_name", sizeof("graph_name"), value)) {
    if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) != IS_STRING) {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "graph_name", "a string" TSRMLS_CC);
      dse_graph_options_free(graph_options);
      return NULL;
    }
    dse_graph_options_set_graph_name_n(graph_options,
                                       PHP5TO7_Z_STRVAL_MAYBE_P(*value),
                                       PHP5TO7_Z_STRLEN_MAYBE_P(*value));
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "read_consistency", sizeof("read_consistency"), value)) {
    long consistency;
    if (php_driver_get_consistency(PHP5TO7_ZVAL_MAYBE_DEREF(value), &consistency TSRMLS_CC) == FAILURE) {
      dse_graph_options_free(graph_options);
      return NULL;
    }

    dse_graph_options_set_read_consistency(graph_options, consistency);
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "write_consistency", sizeof("write_consistency"), value)) {
    long consistency;
    if (php_driver_get_consistency(PHP5TO7_ZVAL_MAYBE_DEREF(value), &consistency TSRMLS_CC) == FAILURE) {
      dse_graph_options_free(graph_options);
      return NULL;
    }

    dse_graph_options_set_write_consistency(graph_options, consistency);
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "request_timeout", sizeof("request_timeout"), value)) {
    cass_int64_t timeout;

    if (!(Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) == IS_LONG   && Z_LVAL_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) > 0) &&
        !(Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) == IS_DOUBLE && Z_DVAL_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) > 0) &&
        !(Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) == IS_NULL)) {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "request_timeout", "a number of seconds greater than zero or null" TSRMLS_CC);
      dse_graph_options_free(graph_options);
      return NULL;
    }

    if ((PHP5TO7_Z_TYPE_MAYBE_P(*value) == IS_LONG && PHP5TO7_Z_LVAL_MAYBE_P(*value) > 0)) {
      timeout = PHP5TO7_Z_LVAL_MAYBE_P(*value) * 1000000;
    } else if ((PHP5TO7_Z_TYPE_MAYBE_P(*value) == IS_DOUBLE && PHP5TO7_Z_DVAL_MAYBE_P(*value) > 0)) {
      timeout = ceil(PHP5TO7_Z_DVAL_MAYBE_P(*value) * 1000000);
    } else {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "request_timeout", "a number of seconds greater than zero or null" TSRMLS_CC);
      dse_graph_options_free(graph_options);
      return NULL;
    }

    dse_graph_options_set_request_timeout(graph_options, timeout);
  }

  return graph_options;
}

static DseGraphStatement *
create_graph(php_driver_session *session,
             const char *query,
             zval *options TSRMLS_DC) {
  php5to7_zval *value;
  DseGraphObject *graph_arguments = NULL;
  DseGraphStatement *graph_statement = NULL;
  DseGraphOptions *graph_options = NULL;

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "arguments", sizeof("arguments"), value)) {
    if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_DEREF(value)) != IS_ARRAY) {
      throw_invalid_argument(PHP5TO7_ZVAL_MAYBE_DEREF(value), "arguments", "an array" TSRMLS_CC);
      return NULL;
    }

    graph_arguments = build_graph_arguments(value TSRMLS_CC);
    if (!graph_arguments) {
      return NULL;
    }
  }

  graph_options = build_graph_options(session, options TSRMLS_CC);
  if (!graph_options) {
    if (!graph_arguments) dse_graph_object_free(graph_arguments);
    return NULL;
  }

  graph_statement = dse_graph_statement_new(query, graph_options);

  if (graph_arguments) {
    ASSERT_SUCCESS_VALUE(dse_graph_statement_bind_values(graph_statement,
                                                         graph_arguments),
                         NULL);
  }

  if (!graph_options) dse_graph_options_free(graph_options);
  if (!graph_arguments) dse_graph_object_free(graph_arguments);

  return graph_statement;
}


PHP_METHOD(DefaultSession, execute)
{
  zval *statement = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  php_driver_statement *stmt = NULL;
  HashTable *arguments = NULL;
  CassConsistency consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  int page_size = -1;
  char *paging_state_token = NULL;
  size_t paging_state_token_size = 0;
  zval *timeout = NULL;
  long serial_consistency = -1;
  CassRetryPolicy *retry_policy = NULL;
  cass_int64_t timestamp = INT64_MIN;
  php_driver_execution_options *opts = NULL;
  CassFuture *future = NULL;
  CassStatement *single = NULL;
  CassBatch *batch  = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());
  stmt = PHP_DRIVER_GET_STATEMENT(statement);

  consistency = self->default_consistency;
  page_size = self->default_page_size;
  timeout = PHP5TO7_ZVAL_MAYBE_P(self->default_timeout);

  if (options) {
    if (!instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC)) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or null");
    }

    opts = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->arguments))
      arguments = PHP5TO7_Z_ARRVAL_MAYBE_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->paging_state_token) {
      paging_state_token = opts->paging_state_token;
      paging_state_token_size = opts->paging_state_token_size;
    }

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->timeout))
      timeout = PHP5TO7_ZVAL_MAYBE_P(opts->timeout);

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->retry_policy))
      retry_policy = (PHP_DRIVER_GET_RETRY_POLICY(PHP5TO7_ZVAL_MAYBE_P(opts->retry_policy)))->policy;

    timestamp = opts->timestamp;
  }

  switch (stmt->type) {
    case PHP_DRIVER_SIMPLE_STATEMENT:
    case PHP_DRIVER_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size,
                             paging_state_token, paging_state_token_size,
                             retry_policy, timestamp TSRMLS_CC);

      if (!single)
        return;

      future = cass_session_execute((CassSession *) self->session->data, single);
      break;
    case PHP_DRIVER_BATCH_STATEMENT:
      batch = create_batch(stmt, consistency, retry_policy, timestamp TSRMLS_CC);

      if (!batch)
        return;

      future = cass_session_execute_batch((CassSession *) self->session->data, batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of " PHP_DRIVER_NAMESPACE "\\SimpleStatement, " \
        PHP_DRIVER_NAMESPACE "\\PreparedStatement or " PHP_DRIVER_NAMESPACE "\\BatchStatement"
      );
      return;
  }

  do {
    const CassResult *result = NULL;
    php_driver_rows *rows = NULL;

    if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE ||
        php_driver_future_is_error(future TSRMLS_CC) == FAILURE)
      break;

    result = cass_future_get_result(future);
    cass_future_free(future);

    if (!result) {
      zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                              "Future doesn't contain a result.");
      break;
    }

    object_init_ex(return_value, php_driver_rows_ce);
    rows = PHP_DRIVER_GET_ROWS(return_value);

    if (php_driver_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
      cass_result_free(result);
      break;
    }

    if (single && cass_result_has_more_pages(result)) {
      rows->statement = php_driver_new_ref(single, free_statement);
      rows->result    = php_driver_new_ref((void *)result, free_result);
      rows->session   = php_driver_add_ref(self->session);
      return;
    }

    cass_result_free(result);
  } while (0);

  if (batch)
    cass_batch_free(batch);

  if (single)
    cass_statement_free(single);
}

PHP_METHOD(DefaultSession, executeAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  php_driver_statement *stmt = NULL;
  HashTable *arguments = NULL;
  CassConsistency consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  int page_size = -1;
  char *paging_state_token = NULL;
  size_t paging_state_token_size = 0;
  long serial_consistency = -1;
  CassRetryPolicy *retry_policy = NULL;
  cass_int64_t timestamp = INT64_MIN;
  php_driver_execution_options *opts = NULL;
  php_driver_future_rows *future_rows = NULL;
  CassStatement *single = NULL;
  CassBatch *batch  = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());
  stmt = PHP_DRIVER_GET_STATEMENT(statement);

  consistency = self->default_consistency;
  page_size = self->default_page_size;

  if (options) {
    if (!instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC)) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or null");
    }

    opts = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->arguments))
      arguments = PHP5TO7_Z_ARRVAL_MAYBE_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->paging_state_token) {
      paging_state_token = opts->paging_state_token;
      paging_state_token_size = opts->paging_state_token_size;
    }

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->retry_policy))
      retry_policy = (PHP_DRIVER_GET_RETRY_POLICY(PHP5TO7_ZVAL_MAYBE_P(opts->retry_policy)))->policy;

    timestamp = opts->timestamp;
  }

  object_init_ex(return_value, php_driver_future_rows_ce);
  future_rows = PHP_DRIVER_GET_FUTURE_ROWS(return_value);

  switch (stmt->type) {
    case PHP_DRIVER_SIMPLE_STATEMENT:
    case PHP_DRIVER_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size,
                             paging_state_token, paging_state_token_size,
                             retry_policy, timestamp TSRMLS_CC);

      if (!single)
        return;

      future_rows->statement = php_driver_new_ref(single, free_statement);
      future_rows->future    = cass_session_execute((CassSession *) self->session->data, single);
      future_rows->session   = php_driver_add_ref(self->session);
      break;
    case PHP_DRIVER_BATCH_STATEMENT:
      batch = create_batch(stmt, consistency, retry_policy, timestamp TSRMLS_CC);

      if (!batch)
        return;

      future_rows->future = cass_session_execute_batch((CassSession *) self->session->data, batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of " PHP_DRIVER_NAMESPACE "\\SimpleStatement, " \
        PHP_DRIVER_NAMESPACE "\\PreparedStatement or " PHP_DRIVER_NAMESPACE "\\BatchStatement"
      );
      return;
  }
}

PHP_METHOD(DefaultSession, prepare)
{
  zval *cql = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  php_driver_execution_options *opts = NULL;
  CassFuture *future = NULL;
  zval *timeout = NULL;
  php_driver_statement *prepared_statement = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (options) {
    if (!instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC)) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or null");
    }

    opts    = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);
    timeout = PHP5TO7_ZVAL_MAYBE_P(opts->timeout);
  }

  future = cass_session_prepare_n((CassSession *)self->session->data,
                                  Z_STRVAL_P(cql), Z_STRLEN_P(cql));

  if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == SUCCESS &&
      php_driver_future_is_error(future TSRMLS_CC) == SUCCESS) {
    object_init_ex(return_value, php_driver_prepared_statement_ce);
    prepared_statement = PHP_DRIVER_GET_STATEMENT(return_value);
    prepared_statement->prepared = cass_future_get_prepared(future);
  }

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, prepareAsync)
{
  zval *cql = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  CassFuture *future = NULL;
  php_driver_future_prepared_statement *future_prepared = NULL;


  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  future = cass_session_prepare_n((CassSession *)self->session->data,
                                  Z_STRVAL_P(cql), Z_STRLEN_P(cql));

  object_init_ex(return_value, php_driver_future_prepared_statement_ce);
  future_prepared = PHP_DRIVER_GET_FUTURE_PREPARED_STATEMENT(return_value);

  future_prepared->future = future;
}

PHP_METHOD(DefaultSession, close)
{
  zval *timeout = NULL;
  CassFuture *future = NULL;
  php_driver_session *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());


  if (self->persist)
    return;

  future = cass_session_close((CassSession *) self->session->data);

  if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == SUCCESS)
    php_driver_future_is_error(future TSRMLS_CC);

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, closeAsync)
{
  php_driver_session *self;
  php_driver_future_close *future = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (self->persist) {
    object_init_ex(return_value, php_driver_future_value_ce);
    return;
  }

  object_init_ex(return_value, php_driver_future_close_ce);
  future = PHP_DRIVER_GET_FUTURE_CLOSE(return_value);

  future->future = cass_session_close((CassSession *) self->session->data);
}

PHP_METHOD(DefaultSession, schema)
{
  php_driver_session *self;
  php_driver_schema *schema;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_SESSION(getThis());

  object_init_ex(return_value, php_driver_default_schema_ce);
  schema = PHP_DRIVER_GET_SCHEMA(return_value);

  schema->schema = php_driver_new_ref(
                     (void *) cass_session_get_schema_meta((CassSession *) self->session->data),
                     free_schema);
}

PHP_METHOD(DefaultSession, executeGraph)
{
  zval *timeout = NULL;
  zval *statement = NULL;
  zval *options = NULL;
  const char *query = NULL;
  DseGraphStatement *graph_statement = NULL;
  php_driver_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(statement) == IS_STRING) {
    query = Z_STRVAL_P(statement);
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(statement), php_driver_graph_simple_statement_ce TSRMLS_CC)) {
    query = PHP_DRIVER_GET_GRAPH_STATEMENT(statement)->query;
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Graph\\SimpleStatement");
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  graph_statement = create_graph(self, query, options TSRMLS_CC);

  if (graph_statement) {
    CassFuture *future = cass_session_execute_dse_graph((CassSession *)self->session->data, graph_statement);
    dse_graph_statement_free(graph_statement);

    if (options) {
      PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "timeout", sizeof("timeout"), timeout);
    }

    do {
      if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE ||
          php_driver_future_is_error(future TSRMLS_CC) == FAILURE) {
        break;
      }

      if (php_driver_graph_result_set_build(future, return_value TSRMLS_CC) == FAILURE) {
        break;
      }
    } while (0);

    cass_future_free(future);
  }
}

PHP_METHOD(DefaultSession, executeGraphAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  const char *query;
  php_driver_session *self = NULL;
  DseGraphStatement *graph_statement = NULL;
  php_driver_graph_future_result_set *future_result_set = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(statement) == IS_STRING) {
    query = Z_STRVAL_P(statement);
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(statement), php_driver_graph_simple_statement_ce TSRMLS_CC)) {
    query = PHP_DRIVER_GET_GRAPH_STATEMENT(statement)->query;
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Graph\\SimpleStatement");
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  graph_statement = create_graph(self, query, options TSRMLS_CC);

  if (graph_statement) {
    object_init_ex(return_value, php_driver_graph_future_result_set_ce);
    future_result_set = PHP_DRIVER_GET_GRAPH_FUTURE_RESULT_SET(return_value);
    future_result_set->future = cass_session_execute_dse_graph((CassSession *)self->session->data, graph_statement);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  PHP_DRIVER_NAMESPACE_ZEND_ARG_OBJ_INFO(0, statement, Statement, 0)
  PHP_DRIVER_NAMESPACE_ZEND_ARG_OBJ_INFO(0, options, ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  PHP_DRIVER_NAMESPACE_ZEND_ARG_OBJ_INFO(0, options, ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute_graph, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_default_session_methods[] = {
  PHP_ME(DefaultSession, execute, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeAsync, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepare, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepareAsync, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, close, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, closeAsync, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, schema, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeGraph, arginfo_execute_graph, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeGraphAsync, arginfo_execute_graph, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_default_session_handlers;

static HashTable *
php_driver_default_session_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_default_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_default_session_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_session *self = PHP5TO7_ZEND_OBJECT_GET(session, object);

  php_driver_del_peref(&self->session, 1);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->default_timeout);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->graph_options);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_default_session_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_session *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(session, ce);

  self->session             = NULL;
  self->persist             = 0;
  self->default_consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  self->default_page_size   = 5000;
  PHP5TO7_ZVAL_UNDEF(self->default_timeout);
  PHP5TO7_ZVAL_UNDEF(self->graph_options);

  PHP5TO7_ZEND_OBJECT_INIT_EX(session, default_session, self, ce);
}

void php_driver_define_DefaultSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DefaultSession", php_driver_default_session_methods);
  php_driver_default_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_default_session_ce TSRMLS_CC, 1, php_driver_session_ce);
  php_driver_default_session_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_default_session_ce->create_object = php_driver_default_session_new;

  memcpy(&php_driver_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_default_session_handlers.get_properties  = php_driver_default_session_properties;
  php_driver_default_session_handlers.compare_objects = php_driver_default_session_compare;
  php_driver_default_session_handlers.clone_obj = NULL;
}
