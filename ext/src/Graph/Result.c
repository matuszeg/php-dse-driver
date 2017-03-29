/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "php_driver_globals.h"

#include <Zend/zend_operators.h>

#include "DefaultEdge.h"
#include "DefaultPath.h"
#include "DefaultVertex.h"
#include "../LineString.h"
#include "../Point.h"
#include "../Polygon.h"

zend_class_entry *php_driver_graph_result_ce = NULL;

static int
parse_string(const DseGraphResult *graph_result, zval* return_value TSRMLS_DC)
{
  CassError rc;
  double x, y;
  size_t len;
  const char *str;

  rc = dse_graph_result_as_point(graph_result, &x, &y);
  if (rc == CASS_OK) {
    php_driver_point *point;

    object_init_ex(return_value, php_driver_point_ce);
    point = PHP_DRIVER_GET_POINT(return_value);
    point->x = x;
    point->y = y;

    return SUCCESS;
  }

  rc = dse_graph_result_as_line_string(graph_result, PHP_DRIVER_G(iterator_line_string));
  if (rc == CASS_OK) {
    object_init_ex(return_value, php_driver_line_string_ce);
    return php_driver_line_string_construct_from_iterator(PHP_DRIVER_G(iterator_line_string),
                                                          return_value TSRMLS_CC);
  }

  rc = dse_graph_result_as_polygon(graph_result, PHP_DRIVER_G(iterator_polygon));
  if (rc == CASS_OK) {
    object_init_ex(return_value, php_driver_polygon_ce);
    return php_driver_polygon_construct_from_iterator(PHP_DRIVER_G(iterator_polygon),
                                                      return_value TSRMLS_CC);
  }

  str = dse_graph_result_get_string(graph_result, &len);
  PHP5TO7_ZVAL_STRINGL(return_value, str, len);

  return SUCCESS;
}

void
to_string(php_driver_graph_result *result, zval *return_value TSRMLS_DC)
{
  if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(result->value)) == IS_OBJECT) {
    if (instanceof_function(PHP5TO7_Z_OBJCE_MAYBE_P(result->value), php_driver_point_ce TSRMLS_CC)) {
      char* wkt = php_driver_point_to_wkt(PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_P(result->value)));
      PHP5TO7_RETVAL_STRING(wkt);
      efree(wkt);
    } else if (instanceof_function(PHP5TO7_Z_OBJCE_MAYBE_P(result->value), php_driver_line_string_ce TSRMLS_CC)) {
      char* wkt = php_driver_line_string_to_wkt(PHP_DRIVER_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_P(result->value)) TSRMLS_CC);
      PHP5TO7_RETVAL_STRING(wkt);
      efree(wkt);
    } else if (instanceof_function(PHP5TO7_Z_OBJCE_MAYBE_P(result->value), php_driver_polygon_ce TSRMLS_CC)) {
      char* wkt = php_driver_polygon_to_wkt(PHP_DRIVER_GET_POLYGON(PHP5TO7_ZVAL_MAYBE_P(result->value)) TSRMLS_CC);
      PHP5TO7_RETVAL_STRING(wkt);
      efree(wkt);
    }
  } else {
    RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(result->value), 1, 0);
    convert_to_string(return_value);
  }
}

int
php_driver_graph_result_construct(const DseGraphResult *graph_result,
                                  zval *return_value TSRMLS_DC)
{
  php_driver_graph_result *result;
  size_t i, count;

  object_init_ex(return_value, php_driver_graph_result_ce);
  result = PHP_DRIVER_GET_GRAPH_RESULT(return_value);

  PHP5TO7_ZVAL_MAYBE_MAKE(result->value);

  result->type = dse_graph_result_type(graph_result);

  switch (result->type) {
  case DSE_GRAPH_RESULT_TYPE_NULL:
    ZVAL_NULL(PHP5TO7_ZVAL_MAYBE_P(result->value));
    break;

  case DSE_GRAPH_RESULT_TYPE_BOOL:
    ZVAL_BOOL(PHP5TO7_ZVAL_MAYBE_P(result->value),
              dse_graph_result_get_bool(graph_result));
    break;

  case DSE_GRAPH_RESULT_TYPE_NUMBER:
    if (dse_graph_result_is_int32(graph_result)) {
      ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(result->value),
                dse_graph_result_get_int32(graph_result));
    } else if (dse_graph_result_is_int64(graph_result)) {
      ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(result->value),
                dse_graph_result_get_int64(graph_result));
    } else {
      ZVAL_DOUBLE(PHP5TO7_ZVAL_MAYBE_P(result->value),
                  dse_graph_result_get_double(graph_result));
    }
    break;

  case DSE_GRAPH_RESULT_TYPE_STRING:
    if (parse_string(graph_result,
                     PHP5TO7_ZVAL_MAYBE_P(result->value) TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }
    break;

  case DSE_GRAPH_RESULT_TYPE_OBJECT:
    array_init(PHP5TO7_ZVAL_MAYBE_P(result->value));
    count = dse_graph_result_member_count(graph_result);
    for (i = 0; i < count; ++i) {
      php5to7_zval value;
      PHP5TO7_ZVAL_MAYBE_MAKE(value);
      if (php_driver_graph_result_construct(dse_graph_result_member_value(graph_result, i),
                                            PHP5TO7_ZVAL_MAYBE_P(value) TSRMLS_CC) == FAILURE) {
        PHP5TO7_ZVAL_MAYBE_DESTROY(value);
        return FAILURE;
      }
      add_assoc_zval(PHP5TO7_ZVAL_MAYBE_P(result->value),
                     dse_graph_result_member_key(graph_result, i, NULL),
                     PHP5TO7_ZVAL_MAYBE_P(value));
    }
    break;

  case DSE_GRAPH_RESULT_TYPE_ARRAY:
    array_init(PHP5TO7_ZVAL_MAYBE_P(result->value));
    count = dse_graph_result_element_count(graph_result);
    for (i = 0; i < count; ++i) {
      php5to7_zval value;
      PHP5TO7_ZVAL_MAYBE_MAKE(value);
      if (php_driver_graph_result_construct(dse_graph_result_element(graph_result, i),
                                            PHP5TO7_ZVAL_MAYBE_P(value) TSRMLS_CC) == FAILURE) {
        PHP5TO7_ZVAL_MAYBE_DESTROY(value);
        return FAILURE;
      }
      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(result->value),
                          PHP5TO7_ZVAL_MAYBE_P(value));
    }
    break;

  default:
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                            "Invalid graph result type");
    return FAILURE;
  }

  return SUCCESS;
}

static int
check_array(php_driver_graph_result *result, const char* message TSRMLS_DC) {
  if ((result->type != DSE_GRAPH_RESULT_TYPE_ARRAY &&
       result->type != DSE_GRAPH_RESULT_TYPE_OBJECT) ||
      PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_ARRAY) {
    zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC, message);
    return FAILURE;
  }
  return SUCCESS;
}

static HashTable *
get_arrval(INTERNAL_FUNCTION_PARAMETERS)
{
  php_driver_graph_result *self = NULL;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  if (check_array(self, "Graph result isn't an array or object" TSRMLS_CC) == FAILURE) {
    return NULL;
  }

  return PHP5TO7_Z_ARRVAL_MAYBE_P(self->value);
}

PHP_METHOD(GraphResult, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\Result objects directly is not supported.");
  return;
}

PHP_METHOD(GraphResult, __toString)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  to_string(self, return_value TSRMLS_CC);
}

PHP_METHOD(GraphResult, count)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  RETURN_LONG(zend_hash_num_elements(arr));
}

PHP_METHOD(GraphResult, rewind)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  zend_hash_internal_pointer_reset(arr);
}

PHP_METHOD(GraphResult, current)
{
  php5to7_zval *entry;
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(arr, entry)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(entry), 1, 0);
  }
}

PHP_METHOD(GraphResult, key)
{
  php5to7_ulong num_index;
  php5to7_string str_index;
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_KEY(arr,
                                        &str_index, &num_index) == HASH_KEY_IS_LONG) {
    RETURN_LONG(num_index);
  }
}

PHP_METHOD(GraphResult, next)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  zend_hash_move_forward(arr);
}

PHP_METHOD(GraphResult, valid)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  RETURN_BOOL(zend_hash_has_more_elements(arr) == SUCCESS);
}

PHP_METHOD(GraphResult, offsetExists)
{
  zval *offset;
  HashTable *arr;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  if (Z_TYPE_P(offset) == IS_LONG || Z_LVAL_P(offset) >= 0) {
    RETURN_BOOL(zend_hash_index_exists(arr, (php5to7_ulong) Z_LVAL_P(offset)));
  } else if (Z_TYPE_P(offset) == IS_STRING) {
    RETURN_BOOL(PHP5TO7_ZEND_HASH_EXISTS(arr, Z_STRVAL_P(offset), Z_STRLEN_P(offset)));
  } else {
    INVALID_ARGUMENT(offset, "a positive integer or a string key");
  }
}

PHP_METHOD(GraphResult, offsetGet)
{
  zval *offset;
  php5to7_zval *value;
  HashTable *arr;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  if (Z_TYPE_P(offset) == IS_LONG && Z_LVAL_P(offset) >= 0) {
    if (PHP5TO7_ZEND_HASH_INDEX_FIND(arr, Z_LVAL_P(offset), value)) {
      RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
    }
  } else if (Z_TYPE_P(offset) == IS_STRING) {
    if (PHP5TO7_ZEND_HASH_FIND(arr, Z_STRVAL_P(offset), Z_STRLEN_P(offset) + 1, value)) {
      RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
    }
  } else {
    INVALID_ARGUMENT(offset, "a positive integer or a string key");
  }

  RETURN_FALSE;
}

PHP_METHOD(GraphResult, offsetSet)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                          "Cannot overwrite a row at a given offset, rows are immutable."
                          );
  return;
}

PHP_METHOD(GraphResult, offsetUnset)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                          "Cannot delete a row at a given offset, rows are immutable."
                          );
  return;
}

PHP_METHOD(GraphResult, isNull)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_NULL);
}

PHP_METHOD(GraphResult, isArray)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_ARRAY);
}

PHP_METHOD(GraphResult, isObject)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_OBJECT);
}

PHP_METHOD(GraphResult, isValue)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_BOOL ||
              self->type == DSE_GRAPH_RESULT_TYPE_NUMBER ||
              self->type == DSE_GRAPH_RESULT_TYPE_STRING);
}

PHP_METHOD(GraphResult, isBool)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_BOOL)
}

PHP_METHOD(GraphResult, isNumber)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_NUMBER)
}

PHP_METHOD(GraphResult, isString)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_STRING)
}

PHP_METHOD(GraphResult, value)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
}

PHP_METHOD(GraphResult, asInt)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_long(return_value);
}

PHP_METHOD(GraphResult, asBool)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_boolean(return_value);
}

PHP_METHOD(GraphResult, asDouble)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_double(return_value);
}

PHP_METHOD(GraphResult, asString)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());

  to_string(self, return_value TSRMLS_CC);
}

PHP_METHOD(GraphResult, asEdge)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());
  if (check_array(self, "Graph result isn't an edge" TSRMLS_CC) == FAILURE) {
    return;
  }

  if (php_driver_graph_default_edge_construct(PHP5TO7_Z_ARRVAL_MAYBE_P(self->value),
                                              return_value TSRMLS_CC) == FAILURE) {
    zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                            "Graph result isn't an edge");
    return;
  }
}

PHP_METHOD(GraphResult, asPath)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());
  if (check_array(self, "Graph result isn't a path" TSRMLS_CC) == FAILURE) {
    return;
  }

  if (php_driver_graph_default_path_construct(PHP5TO7_Z_ARRVAL_MAYBE_P(self->value),
                                              return_value TSRMLS_CC) == FAILURE) {
    zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                            "Graph result isn't a path");
    return;
  }
}

PHP_METHOD(GraphResult, asVertex)
{
  php_driver_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis());
  if (check_array(self, "Graph result isn't a vertex" TSRMLS_CC) == FAILURE) {
    return;
  }

  if (php_driver_graph_default_vertex_construct(PHP5TO7_Z_ARRVAL_MAYBE_P(self->value),
                                                return_value TSRMLS_CC) == FAILURE) {
    zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                            "Graph result isn't a vertex");
    return;
  }
}

#define XX_DSE_METHOD(type_name, _, type_name_proper, __) \
PHP_METHOD(GraphResult, as##type_name_proper) { \
  php_driver_graph_result *self = NULL; \
  if (zend_parse_parameters_none() == FAILURE) \
    return; \
  self = PHP_DRIVER_GET_GRAPH_RESULT(getThis()); \
  if (!instanceof_function(PHP5TO7_Z_OBJCE_MAYBE_P(self->value), \
                           php_driver_##type_name##_ce TSRMLS_CC)) { \
    zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC, \
                            "Graph result isn't a " #type_name); \
  } \
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0); \
}

PHP_DRIVER_DSE_TYPES_MAP(XX_DSE_METHOD)
#undef XX_DSE_METHOD

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offset, 0, ZEND_RETURN_VALUE, 1)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, ZEND_RETURN_VALUE, 2)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_result_methods[] = {
  PHP_ME(GraphResult, __construct,  arginfo_none,   ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(GraphResult, __toString,   arginfo_none,   ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(GraphResult, count,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, rewind,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, current,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, key,          arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, next,         arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, valid,        arginfo_none,   ZEND_ACC_PUBLIC)
  /* Array access */
  PHP_ME(GraphResult, offsetExists, arginfo_offset, ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, offsetGet,    arginfo_offset, ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, offsetSet,    arginfo_set,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, offsetUnset,  arginfo_offset, ZEND_ACC_PUBLIC)
  /* Graph result */
  PHP_ME(GraphResult, isNull,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isValue,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isArray,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isObject,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isBool,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isNumber,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, isString,     arginfo_none,   ZEND_ACC_PUBLIC)
  /* Basic types */
  PHP_ME(GraphResult, value,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asInt,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asBool,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asDouble,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asString,     arginfo_none,   ZEND_ACC_PUBLIC)
  /* Graph types */
  PHP_ME(GraphResult, asEdge,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asPath,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(GraphResult, asVertex,     arginfo_none,   ZEND_ACC_PUBLIC)
  /* DSE types */
#define XX_DSE_METHOD(_, __, name, ___) PHP_ME(GraphResult, as##name, arginfo_none, ZEND_ACC_PUBLIC)

  PHP_DRIVER_DSE_TYPES_MAP(XX_DSE_METHOD)
#undef XX_DSE_METHOD
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_result_handlers;

static HashTable *
php_driver_graph_result_properties(zval *object TSRMLS_DC)
{
  const char *typestr = "unknown";
  php5to7_zval value;
  php_driver_graph_result *self  = PHP_DRIVER_GET_GRAPH_RESULT(object);
  HashTable               *props = zend_std_get_properties(object TSRMLS_CC);

  switch(self->type) {
  case DSE_GRAPH_RESULT_TYPE_NULL:
    typestr = "null";
    break;
  case DSE_GRAPH_RESULT_TYPE_BOOL:
    typestr = "bool";
    break;
  case DSE_GRAPH_RESULT_TYPE_NUMBER:
    typestr = "number";
    break;
  case DSE_GRAPH_RESULT_TYPE_STRING:
    typestr = "string";
    break;
  case DSE_GRAPH_RESULT_TYPE_OBJECT:
    typestr = "object";
    break;
  case DSE_GRAPH_RESULT_TYPE_ARRAY:
    typestr = "array";
    break;
  }

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(value), typestr);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "type", sizeof("type"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "value", sizeof("value"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  return props;
}

static int
php_driver_graph_result_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_result_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_result *self = PHP5TO7_ZEND_OBJECT_GET(graph_result, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->value);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_result_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_result *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_result, ce);

  PHP5TO7_ZVAL_UNDEF(self->value);

  PHP5TO7_ZEND_OBJECT_INIT(graph_result, self, ce);
}

void php_driver_define_GraphResult(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Result", php_driver_graph_result_methods);
  php_driver_graph_result_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_result_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
  php_driver_graph_result_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_result_ce->create_object = php_driver_graph_result_new;

  memcpy(&php_driver_graph_result_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_result_handlers.get_properties  = php_driver_graph_result_properties;
  php_driver_graph_result_handlers.compare_objects = php_driver_graph_result_compare;
  php_driver_graph_result_handlers.clone_obj = NULL;
}
