#include "php_dse.h"
#include "php_dse_types.h"

#include "util/collections.h"
#include "util/consistency.h"
#include "util/future.h"
#include "util/hash.h"
#include "util/math.h"
#include "Cassandra/Session.h"
#include "Graph/ResultSet.h"
#include "LineString.h"
#include "Point.h"
#include "Polygon.h"

#include <ext/standard/base64.h>

zend_class_entry *dse_default_session_ce = NULL;

PHP_METHOD(DseDefaultSession, execute)
{
  zval *statement = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

  php_cassandra_session_execute(&self->base, statement, options, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, executeAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

  php_cassandra_session_execute_async(&self->base, statement, options, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, prepare)
{
  zval *cql = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

  php_cassandra_session_prepare(&self->base, cql, options, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, prepareAsync)
{
  zval *cql = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

  php_cassandra_session_prepare_async(&self->base, cql, options, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, close)
{
  zval *timeout = NULL;

  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  php_cassandra_session_close(&self->base, timeout, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, closeAsync)
{
  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  php_cassandra_session_close_async(&self->base, return_value TSRMLS_CC);
}

PHP_METHOD(DseDefaultSession, schema)
{
  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters_none() == FAILURE)
    return;

  php_cassandra_session_schema(&self->base, return_value TSRMLS_CC);
}

#define CHECK_RESULT(rc) \
{ \
  ASSERT_SUCCESS_VALUE(rc, FAILURE) \
  return SUCCESS; \
}

static int graph_array_from_set(cassandra_set *set,
                                DseGraphArray **result TSRMLS_DC);

static int graph_array_from_collection(cassandra_collection *coll,
                                       DseGraphArray **result TSRMLS_DC);

static int graph_object_from_map(cassandra_map *map,
                                 DseGraphObject **result TSRMLS_DC);

static int graph_array_from_tuple(cassandra_tuple *tuple,
                                  DseGraphArray **result TSRMLS_DC);

static int graph_object_from_user_type_value(cassandra_user_type_value *user_type_value,
                                             DseGraphObject **result TSRMLS_DC);

static int graph_build_array(zval *value,
                             DseGraphArray **result TSRMLS_DC);

static int graph_build_object(zval *value,
                              DseGraphObject **result TSRMLS_DC);

static int graph_object_add_uuid(DseGraphObject *object,
                                 const char *name,
                                 cassandra_uuid *uuid TSRMLS_DC)
{
  char uuid_str[CASS_UUID_STRING_LENGTH];
  cass_uuid_string(uuid->uuid, uuid_str);
  CHECK_RESULT(dse_graph_object_add_string(object, name, uuid_str));
}

static int graph_array_add_uuid(DseGraphArray *array,
                                cassandra_uuid *uuid TSRMLS_DC)
{
  char uuid_str[CASS_UUID_STRING_LENGTH];
  cass_uuid_string(uuid->uuid, uuid_str);
  CHECK_RESULT(dse_graph_array_add_string(array, uuid_str));
}

static int graph_object_add_inet(DseGraphObject *object,
                                 const char *name,
                                 cassandra_inet *inet TSRMLS_DC)
{
  char inet_str[CASS_INET_STRING_LENGTH];
  cass_inet_string(inet->inet, inet_str);
  CHECK_RESULT(dse_graph_object_add_string(object, name, inet_str));
}

static int graph_array_add_inet(DseGraphArray *array,
                                cassandra_inet *inet TSRMLS_DC)
{
  char inet_str[CASS_INET_STRING_LENGTH];
  cass_inet_string(inet->inet, inet_str);
  CHECK_RESULT(dse_graph_array_add_string(array, inet_str));
}

static int graph_object_add_varint(DseGraphObject *object,
                                   const char *name,
                                   cassandra_numeric *varint TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_cassandra_format_integer(varint->varint_value, &str, &len);
  rc = dse_graph_object_add_string_n(object,
                                     name, strlen(name),
                                     str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int graph_array_add_varint(DseGraphArray *array,
                                  cassandra_numeric *varint TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_cassandra_format_integer(varint->varint_value, &str, &len);
  rc = dse_graph_array_add_string_n(array, str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int graph_object_add_decimal(DseGraphObject *object,
                                   const char *name,
                                   cassandra_numeric *decimal TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_cassandra_format_decimal(decimal->decimal_value, decimal->decimal_scale,
                               &str, &len);
  rc = dse_graph_object_add_string_n(object,
                                     name, strlen(name),
                                     str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int graph_array_add_decimal(DseGraphArray *array,
                                  cassandra_numeric *decimal TSRMLS_DC)
{
  char *str;
  int len;
  CassError rc;
  php_cassandra_format_decimal(decimal->decimal_value, decimal->decimal_scale,
                               &str, &len);
  rc = dse_graph_array_add_string_n(array, str, len);
  efree(str);
  CHECK_RESULT(rc);
}

static int graph_object_add_blob(DseGraphObject *object,
                                 const char *name,
                                 cassandra_blob *blob TSRMLS_DC)
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

static int graph_array_add_blob(DseGraphArray *array,
                                cassandra_blob *blob TSRMLS_DC)
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

static int graph_object_add_with_value_type(DseGraphObject *object,
                                            const char *name,
                                            zval *value,
                                            CassValueType type TSRMLS_DC)
{
  CassError             rc;
  cassandra_blob       *blob;
  cassandra_numeric    *numeric;
  cassandra_timestamp  *timestamp;
  cassandra_date       *date;
  cassandra_time       *time;
  cassandra_uuid       *uuid;
  cassandra_inet       *inet;
  cassandra_collection *coll;
  cassandra_map        *map;
  cassandra_set        *set;
  cassandra_tuple      *tuple;
  cassandra_user_type_value *user_type_value;
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
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_SMALL_INT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int32(object, name, numeric->smallint_value));
    break;
  case CASS_VALUE_TYPE_TINY_INT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_int32(object, name, numeric->tinyint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_CASSANDRA_GET_BLOB(value);
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
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_object_add_double(object, name, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_RESULT(dse_graph_object_add_int32(object, name, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_DATE:
    date = PHP_CASSANDRA_GET_DATE(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, date->date));
    break;
  case CASS_VALUE_TYPE_TIME:
    time = PHP_CASSANDRA_GET_TIME(value);
    CHECK_RESULT(dse_graph_object_add_int64(object, name, time->time));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_CASSANDRA_GET_UUID(value);
    return graph_object_add_uuid(object, name, uuid TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    return graph_object_add_varint(object, name, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    return graph_object_add_decimal(object, name, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_CASSANDRA_GET_INET(value);
    return graph_object_add_inet(object, name, inet TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_CASSANDRA_GET_COLLECTION(value);
    if (graph_array_from_collection(coll, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_CASSANDRA_GET_MAP(value);
    if (graph_object_from_map(map, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_object(object, name, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_CASSANDRA_GET_SET(value);
    if (graph_array_from_set(set, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_TUPLE:
    tuple = PHP_CASSANDRA_GET_TUPLE(value);
    if (graph_array_from_tuple(tuple, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_array(object, name, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
    if (graph_object_from_user_type_value(user_type_value, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_object_add_object(object, name, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return FAILURE;
  }

  return FAILURE;
}

static int graph_array_add_with_value_type(DseGraphArray *array,
                                           zval *value,
                                           CassValueType type TSRMLS_DC)
{
  CassError             rc;
  cassandra_blob       *blob;
  cassandra_numeric    *numeric;
  cassandra_timestamp  *timestamp;
  cassandra_date       *date;
  cassandra_time       *time;
  cassandra_uuid       *uuid;
  cassandra_inet       *inet;
  cassandra_collection *coll;
  cassandra_map        *map;
  cassandra_set        *set;
  cassandra_tuple      *tuple;
  cassandra_user_type_value *user_type_value;
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
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_SMALL_INT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int32(array, numeric->smallint_value));
    break;
  case CASS_VALUE_TYPE_TINY_INT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_int32(array, numeric->tinyint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_CASSANDRA_GET_BLOB(value);
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
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_RESULT(dse_graph_array_add_double(array, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_RESULT(dse_graph_array_add_int32(array, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_DATE:
    date = PHP_CASSANDRA_GET_DATE(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, date->date));
    break;
  case CASS_VALUE_TYPE_TIME:
    time = PHP_CASSANDRA_GET_TIME(value);
    CHECK_RESULT(dse_graph_array_add_int64(array, time->time));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_CASSANDRA_GET_UUID(value);
    return graph_array_add_uuid(array, uuid TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    return graph_array_add_varint(array, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    return graph_array_add_decimal(array, numeric TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_CASSANDRA_GET_INET(value);
    return graph_array_add_inet(array, inet TSRMLS_CC);
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_CASSANDRA_GET_COLLECTION(value);
    if (graph_array_from_collection(coll, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_CASSANDRA_GET_MAP(value);
    if (graph_object_from_map(map, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_object(array, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_CASSANDRA_GET_SET(value);
    if (graph_array_from_set(set, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_TUPLE:
    tuple = PHP_CASSANDRA_GET_TUPLE(value);
    if (graph_array_from_tuple(tuple, &sub_array TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_array(array, sub_array);
    dse_graph_array_free(sub_array);
    CHECK_RESULT(rc);
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
    if (graph_object_from_user_type_value(user_type_value, &sub_object TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    rc = dse_graph_array_add_object(array, sub_object);
    dse_graph_object_free(sub_object);
    CHECK_RESULT(rc);
    break;
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return FAILURE;
  }

  return FAILURE;
}

static int graph_array_from_set(cassandra_set *set,
                                DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  cassandra_type *type;
  cassandra_type *value_type;
  DseGraphArray *array;
  cassandra_set_entry *curr, *temp;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
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

static int graph_array_from_collection(cassandra_collection *coll,
                                       DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  cassandra_type *type;
  cassandra_type *value_type;
  DseGraphArray *array;
  php5to7_zval *curr;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(coll->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
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

static int graph_object_from_map(cassandra_map *map,
                                 DseGraphObject **result TSRMLS_DC)
{
  int rc = SUCCESS;
  cassandra_type *type;
  cassandra_type *key_type;
  cassandra_type *value_type;
  DseGraphObject *object;
  cassandra_map_entry *curr, *temp;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
  key_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->key_type));
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

static int graph_array_from_tuple(cassandra_tuple *tuple,
                                  DseGraphArray **result TSRMLS_DC)
{
  int rc = SUCCESS;
  php5to7_ulong num_key;
  php5to7_zval *current;
  cassandra_type *type;
  DseGraphArray *array;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(tuple->type));
  array = dse_graph_array_new();

  PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(&tuple->values, num_key, current) {
    php5to7_zval *zsub_type;
    cassandra_type *sub_type;
    PHP5TO7_ZEND_HASH_INDEX_FIND(&type->types, num_key, zsub_type);
    if (!php_cassandra_validate_object(PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                       PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type) TSRMLS_CC)) {
      rc = FAILURE;
      break;
    }
    sub_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type));
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

static int graph_object_from_user_type_value(cassandra_user_type_value *user_type_value,
                                             DseGraphObject **result TSRMLS_DC)
{
  int rc = SUCCESS;
  char *name;
  php5to7_zval *current;
  cassandra_type *type;
  DseGraphObject *object;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value->type));
  object = dse_graph_object_new();

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(&user_type_value->values, name, current) {
    php5to7_zval *zsub_type;
    cassandra_type *sub_type;
    if (!PHP5TO7_ZEND_HASH_FIND(&type->types, name, strlen(name) + 1, zsub_type) ||
        !php_cassandra_validate_object(PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                       PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type) TSRMLS_CC)) {
      rc = FAILURE;
      break;
    }
    sub_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type));
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

static int graph_determine_array_type(zval *value) {
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

static int graph_array_add(DseGraphArray *array,
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
    if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
      cassandra_numeric *float_number = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_double(array, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
      cassandra_numeric *bigint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_smallint_ce TSRMLS_CC)) {
      cassandra_numeric *smallint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int32(array, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_tinyint_ce TSRMLS_CC)) {
      cassandra_numeric *tinyint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_array_add_int32(array, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
      cassandra_timestamp *timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_date_ce TSRMLS_CC)) {
      cassandra_date *date = PHP_CASSANDRA_GET_DATE(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_time_ce TSRMLS_CC)) {
      cassandra_time *time = PHP_CASSANDRA_GET_TIME(value);
      CHECK_RESULT(dse_graph_array_add_int64(array, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
      cassandra_blob *blob = PHP_CASSANDRA_GET_BLOB(value);
      return graph_array_add_blob(array, blob TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
      cassandra_numeric *varint = PHP_CASSANDRA_GET_NUMERIC(value);
      return graph_array_add_varint(array, varint TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
      cassandra_numeric *decimal = PHP_CASSANDRA_GET_NUMERIC(value);
      return graph_array_add_decimal(array, decimal TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_interface_ce TSRMLS_CC)) {
      cassandra_uuid *uuid = PHP_CASSANDRA_GET_UUID(value);
      return graph_array_add_uuid(array, uuid TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
      cassandra_inet *inet = PHP_CASSANDRA_GET_INET(value);
      return graph_array_add_inet(array, inet TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_set *set = PHP_CASSANDRA_GET_SET(value);
      if (graph_array_from_set(set, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      cassandra_map *map = PHP_CASSANDRA_GET_MAP(value);
      if (graph_object_from_map(map, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_object(array, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_collection *coll = PHP_CASSANDRA_GET_COLLECTION(value);
      if (graph_array_from_collection(coll, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_tuple_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_tuple *tuple = PHP_CASSANDRA_GET_TUPLE(value);
      if (graph_array_from_tuple(tuple, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_array(array, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_user_type_value_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      cassandra_user_type_value *user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
      if (graph_object_from_user_type_value(user_type_value, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_array_add_object(array, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_line_string_ce TSRMLS_CC)) {
      char* wkt = line_string_to_wkt(PHP_DSE_GET_LINE_STRING(value) TSRMLS_CC);
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_point_ce TSRMLS_CC)) {
      char* wkt = point_to_wkt(PHP_DSE_GET_POINT(value));
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_polygon_ce TSRMLS_CC)) {
      char* wkt = polygon_to_wkt(PHP_DSE_GET_POLYGON(value) TSRMLS_CC);
      CassError rc = dse_graph_array_add_string(array, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int graph_object_add(DseGraphObject *object,
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
    if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
      cassandra_numeric *float_number = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_double(object, name, float_number->float_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
      cassandra_numeric *bigint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, bigint->bigint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_smallint_ce TSRMLS_CC)) {
      cassandra_numeric *smallint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int32(object, name, smallint->smallint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_tinyint_ce TSRMLS_CC)) {
      cassandra_numeric *tinyint = PHP_CASSANDRA_GET_NUMERIC(value);
      CHECK_RESULT(dse_graph_object_add_int32(object, name, tinyint->tinyint_value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
      cassandra_timestamp *timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_date_ce TSRMLS_CC)) {
      cassandra_date *date = PHP_CASSANDRA_GET_DATE(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_time_ce TSRMLS_CC)) {
      cassandra_time *time = PHP_CASSANDRA_GET_TIME(value);
      CHECK_RESULT(dse_graph_object_add_int64(object, name, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
      cassandra_blob *blob = PHP_CASSANDRA_GET_BLOB(value);
      return graph_object_add_blob(object, name, blob TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
      cassandra_numeric *varint = PHP_CASSANDRA_GET_NUMERIC(value);
      return graph_object_add_varint(object, name, varint TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
      cassandra_numeric *decimal = PHP_CASSANDRA_GET_NUMERIC(value);
      return graph_object_add_decimal(object, name, decimal TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_interface_ce TSRMLS_CC)) {
      cassandra_uuid *uuid = PHP_CASSANDRA_GET_UUID(value);
      return graph_object_add_uuid(object, name, uuid TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
      cassandra_inet *inet = PHP_CASSANDRA_GET_INET(value);
      return graph_object_add_inet(object, name, inet TSRMLS_CC);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_set *set = PHP_CASSANDRA_GET_SET(value);
      if (graph_array_from_set(set, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      cassandra_map *map = PHP_CASSANDRA_GET_MAP(value);
      if (graph_object_from_map(map, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_object(object, name, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_collection *coll = PHP_CASSANDRA_GET_COLLECTION(value);
      if (graph_array_from_collection(coll, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_tuple_ce TSRMLS_CC)) {
      DseGraphArray *array;
      CassError rc;
      cassandra_tuple *tuple = PHP_CASSANDRA_GET_TUPLE(value);
      if (graph_array_from_tuple(tuple, &array TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_array(object, name, array);
      dse_graph_array_free(array);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_user_type_value_ce TSRMLS_CC)) {
      DseGraphObject *object;
      CassError rc;
      cassandra_user_type_value *user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
      if (graph_object_from_user_type_value(user_type_value, &object TSRMLS_CC) == FAILURE) {
        return FAILURE;
      }
      rc = dse_graph_object_add_object(object, name, object);
      dse_graph_object_free(object);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_line_string_ce TSRMLS_CC)) {
      char* wkt = line_string_to_wkt(PHP_DSE_GET_LINE_STRING(value) TSRMLS_CC);
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_point_ce TSRMLS_CC)) {
      char* wkt = point_to_wkt(PHP_DSE_GET_POINT(value));
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), dse_polygon_ce TSRMLS_CC)) {
      char* wkt = polygon_to_wkt(PHP_DSE_GET_POLYGON(value) TSRMLS_CC);
      CassError rc = dse_graph_object_add_string(object, name, wkt);
      efree(wkt);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int graph_build_array(zval *value,
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

static int graph_build_object(zval *value,
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

DseGraphObject *build_graph_arguments(php5to7_zval *arguments TSRMLS_DC) {
  char *name;
  php5to7_zval *current;
  DseGraphObject *result = dse_graph_object_new();

  HashTable *array = PHP5TO7_Z_ARRVAL_MAYBE_P(*arguments);

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(array, name, current) {
    if (!name) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
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

DseGraphOptions *build_graph_options(dse_session *session,
                                     zval *options TSRMLS_DC) {
  php5to7_zval *value;
  DseGraphOptions *graph_options;

  if (!PHP5TO7_ZVAL_IS_UNDEF(session->graph_options)) {
    graph_options = dse_graph_options_new_from_existing(
                      PHP_DSE_GET_GRAPH_OPTIONS(
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
    if (php_cassandra_get_consistency(PHP5TO7_ZVAL_MAYBE_DEREF(value), &consistency TSRMLS_CC) == FAILURE) {
      dse_graph_options_free(graph_options);
      return NULL;
    }

    dse_graph_options_set_read_consistency(graph_options, consistency);
  }

  if (options && PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "write_consistency", sizeof("write_consistency"), value)) {
    long consistency;
    if (php_cassandra_get_consistency(PHP5TO7_ZVAL_MAYBE_DEREF(value), &consistency TSRMLS_CC) == FAILURE) {
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

DseGraphStatement *create_graph(dse_session *session,
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

PHP_METHOD(DseDefaultSession, executeGraph)
{
  zval *timeout = NULL;
  zval *statement = NULL;
  zval *options = NULL;
  const char *query = NULL;
  DseGraphStatement *graph_statement = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(statement) == IS_STRING) {
    query = Z_STRVAL_P(statement);
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(statement), dse_graph_simple_statement_ce TSRMLS_CC)) {
    query = PHP_DSE_GET_GRAPH_STATEMENT(statement)->query;
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of Dse\\Graph\\SimpleStatement");
  }

  self = PHP_DSE_GET_SESSION(getThis());

  graph_statement = create_graph(self, query, options TSRMLS_CC);

  if (graph_statement) {
    CassFuture *future = cass_session_execute_dse_graph((CassSession *)self->base.session->data, graph_statement);
    dse_graph_statement_free(graph_statement);

    if (options) {
      PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "timeout", sizeof("timeout"), timeout);
    }

    do {
      if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE ||
          php_cassandra_future_is_error(future TSRMLS_CC) == FAILURE) {
        break;
      }

      if (php_dse_graph_result_set_build(future, return_value TSRMLS_CC) == FAILURE) {
        break;
      }
    } while (0);

    cass_future_free(future);
  }
}

PHP_METHOD(DseDefaultSession, executeGraphAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  const char *query;
  dse_session *self = NULL;
  DseGraphStatement *graph_statement = NULL;
  dse_graph_future_result_set *future_result_set = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(statement) == IS_STRING) {
    query = Z_STRVAL_P(statement);
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(statement), dse_graph_simple_statement_ce TSRMLS_CC)) {
    query = PHP_DSE_GET_GRAPH_STATEMENT(statement)->query;
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of Dse\\Graph\\SimpleStatement");
  }

  self = PHP_DSE_GET_SESSION(getThis());

  graph_statement = create_graph(self, query, options TSRMLS_CC);

  if (graph_statement) {
    object_init_ex(return_value, dse_graph_future_result_set_ce);
    future_result_set = PHP_DSE_GET_GRAPH_FUTURE_RESULT_SET(return_value);
    future_result_set->future = cass_session_execute_dse_graph((CassSession *)self->base.session->data, graph_statement);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, statement, Cassandra\\Statement, 0)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute_graph, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry dse_default_session_methods[] = {
  PHP_ME(DseDefaultSession, execute, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, executeAsync, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, prepare, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, prepareAsync, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, close, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, closeAsync, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, schema, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, executeGraph, arginfo_execute_graph, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, executeGraphAsync, arginfo_execute_graph, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_default_session_handlers;

static HashTable *
php_dse_default_session_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_default_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_default_session_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_session *self = PHP5TO7_ZEND_OBJECT_GET(dse_session, object);

  php_cassandra_session_destroy(&self->base);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->graph_options);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_default_session_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_session *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_session, ce);

  php_cassandra_session_init(&self->base);
  PHP5TO7_ZVAL_UNDEF(self->graph_options);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_session, dse_default_session, self, ce);
}

void dse_define_DefaultSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\DefaultSession", dse_default_session_methods);
  dse_default_session_ce = php5to7_zend_register_internal_class_ex(&ce, dse_session_ce);
  dse_default_session_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_default_session_ce->create_object = php_dse_default_session_new;

  memcpy(&dse_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_default_session_handlers.get_properties  = php_dse_default_session_properties;
  dse_default_session_handlers.compare_objects = php_dse_default_session_compare;
  dse_default_session_handlers.clone_obj = NULL;
}
