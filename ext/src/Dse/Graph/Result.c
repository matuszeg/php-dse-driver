#include "php_dse.h"
#include "php_dse_types.h"

#include "Zend/zend_operators.h"

zend_class_entry *dse_graph_result_ce = NULL;

int
php_dse_graph_result_build(const DseGraphResult *graph_result,
                           zval *return_value TSRMLS_DC)
{
  dse_graph_result *result;
  size_t i, count;
  zval *value;
  DseGraphResultType type = dse_graph_result_type(graph_result);

  object_init_ex(return_value, dse_graph_result_ce);
  result = PHP_DSE_GET_GRAPH_RESULT(return_value);

  PHP5TO7_ZVAL_MAYBE_MAKE(result->value);

  result->type = type;

  switch (type) {
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
    PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(result->value),
                        dse_graph_result_get_string(graph_result, NULL));
    break;

  case DSE_GRAPH_RESULT_TYPE_OBJECT:
    array_init(PHP5TO7_ZVAL_MAYBE_P(result->value));
    count = dse_graph_result_member_count(graph_result);
    for (i = 0; i < count; ++i) {
      PHP5TO7_ZVAL_MAYBE_MAKE(value);
      if (php_dse_graph_result_build(dse_graph_result_member_value(graph_result, i),
                                     value TSRMLS_CC) == FAILURE) {
        PHP5TO7_ZVAL_MAYBE_DESTROY(value);
        return FAILURE;
      }
      add_assoc_zval(PHP5TO7_ZVAL_MAYBE_P(result->value),
                     dse_graph_result_member_key(graph_result, i, NULL),
                     value);
    }
    break;

  case DSE_GRAPH_RESULT_TYPE_ARRAY:
    array_init(PHP5TO7_ZVAL_MAYBE_P(result->value));
    count = dse_graph_result_element_count(graph_result);
    for (i = 0; i < count; ++i) {
      PHP5TO7_ZVAL_MAYBE_MAKE(value);
      if (php_dse_graph_result_build(dse_graph_result_element(graph_result, i),
                                     value TSRMLS_CC) == FAILURE) {
        PHP5TO7_ZVAL_MAYBE_DESTROY(value);
        return FAILURE;
      }
      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(result->value), value);
    }
    break;

  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
                            "Invalid graph result type");
    return FAILURE;
  }

  return SUCCESS;
}

PHP_METHOD(DseGraphResult, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\Result objects directly is not supported.");
  return;
}

static HashTable *
get_arrval(INTERNAL_FUNCTION_PARAMETERS)
{
  dse_graph_result *self = NULL;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  if (self->type != DSE_GRAPH_RESULT_TYPE_ARRAY ||
      self->type != DSE_GRAPH_RESULT_TYPE_OBJECT ||
      PHP5TO7_Z_TYPE_MAYBE_P(self->value) != IS_ARRAY) {
    zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
                            "Graph result isn't an array or object");
    return NULL;
  }

  return PHP5TO7_Z_ARRVAL_MAYBE_P(self->value);
}

PHP_METHOD(DseGraphResult, count)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  RETURN_LONG(zend_hash_num_elements(arr));
}

PHP_METHOD(DseGraphResult, rewind)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  zend_hash_internal_pointer_reset(arr);
}

PHP_METHOD(DseGraphResult, current)
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

PHP_METHOD(DseGraphResult, key)
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

PHP_METHOD(DseGraphResult, next)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  zend_hash_move_forward(arr);
}

PHP_METHOD(DseGraphResult, valid)
{
  HashTable *arr;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  RETURN_BOOL(zend_hash_has_more_elements(arr) == SUCCESS);
}

PHP_METHOD(DseGraphResult, offsetExists)
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
    RETURN_BOOL(zend_hash_exists(arr, Z_STRVAL_P(offset), Z_STRLEN_P(offset)));
  } else {
    INVALID_ARGUMENT(offset, "a positive integer or a string key");
  }
}

PHP_METHOD(DseGraphResult, offsetGet)
{
  zval *offset;
  php5to7_zval *value;
  HashTable *arr;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  arr = get_arrval(INTERNAL_FUNCTION_PARAM_PASSTHRU);
  if (!arr) return;

  if (Z_TYPE_P(offset) == IS_LONG || Z_LVAL_P(offset) >= 0) {
    if (PHP5TO7_ZEND_HASH_INDEX_FIND(arr, Z_LVAL_P(offset), value)) {
      RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
    }
  } else if (Z_TYPE_P(offset) == IS_STRING) {
    if (PHP5TO7_ZEND_HASH_FIND(arr, Z_STRVAL_P(offset), Z_STRLEN_P(offset), value)) {
      RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
    }
  } else {
    INVALID_ARGUMENT(offset, "a positive integer or a string key");
  }

}

PHP_METHOD(DseGraphResult, offsetSet)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
    "Cannot overwrite a row at a given offset, rows are immutable."
  );
  return;
}

PHP_METHOD(DseGraphResult, offsetUnset)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
    "Cannot delete a row at a given offset, rows are immutable."
  );
  return;
}

PHP_METHOD(DseGraphResult, isNull)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_NULL);
}

PHP_METHOD(DseGraphResult, isArray)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_ARRAY);
}

PHP_METHOD(DseGraphResult, isObject)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_OBJECT);
}

PHP_METHOD(DseGraphResult, isValue)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_BOOL ||
              self->type == DSE_GRAPH_RESULT_TYPE_NUMBER ||
              self->type == DSE_GRAPH_RESULT_TYPE_STRING);
}

PHP_METHOD(DseGraphResult, isBool)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_BOOL)
}

PHP_METHOD(DseGraphResult, isNumber)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_NUMBER)
}

PHP_METHOD(DseGraphResult, isString)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETURN_BOOL(self->type == DSE_GRAPH_RESULT_TYPE_STRING)
}

PHP_METHOD(DseGraphResult, asInt)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_long(return_value);
}

PHP_METHOD(DseGraphResult, asBoolean)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_boolean(return_value);
}

PHP_METHOD(DseGraphResult, asDouble)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_double(return_value);
}

PHP_METHOD(DseGraphResult, asString)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
  convert_to_string(return_value);
}

PHP_METHOD(DseGraphResult, asArray)
{
  dse_graph_result *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_RESULT(getThis());

  if (self->type != DSE_GRAPH_RESULT_TYPE_ARRAY ||
      self->type != DSE_GRAPH_RESULT_TYPE_OBJECT ||
      PHP5TO7_Z_TYPE_MAYBE_P(self->value) != IS_ARRAY) {
    zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
                            "Graph result isn't an array or object");
    return;
  }

  RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value), 1, 0);
}

PHP_METHOD(DseGraphResult, asPoint)
{
}

PHP_METHOD(DseGraphResult, asLineString)
{
}

PHP_METHOD(DseGraphResult, asPolygon)
{
}

PHP_METHOD(DseGraphResult, asVertex)
{
}

PHP_METHOD(DseGraphResult, asEdge)
{
}

PHP_METHOD(DseGraphResult, asPath)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offset, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, offset)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_result_methods[] = {
  PHP_ME(DseGraphResult, __construct,  arginfo_none,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  /* Iterator */
  PHP_ME(DseGraphResult, count,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, rewind,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, current,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, key,          arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, next,         arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, valid,        arginfo_none,   ZEND_ACC_PUBLIC)
  /* Array access */
  PHP_ME(DseGraphResult, offsetExists, arginfo_offset, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, offsetGet,    arginfo_offset, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, offsetSet,    arginfo_set,    ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, offsetUnset,  arginfo_offset, ZEND_ACC_PUBLIC)
  /* Graph result */
  PHP_ME(DseGraphResult, isNull,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, isValue,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, isArray,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, isObject,     arginfo_none,   ZEND_ACC_PUBLIC)
  /* Basic types */
  PHP_ME(DseGraphResult, asInt,        arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asBoolean,    arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asDouble,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asString,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asArray,      arginfo_none,   ZEND_ACC_PUBLIC)
  /* Geometric types */
  PHP_ME(DseGraphResult, asPoint,      arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asLineString, arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asPolygon,    arginfo_none,   ZEND_ACC_PUBLIC)
  /* Graph types */
  PHP_ME(DseGraphResult, asVertex,     arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asEdge,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphResult, asPath,       arginfo_none,   ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_result_handlers;

static HashTable *
php_dse_graph_result_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_graph_result_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_result_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_result *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_result, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->value);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_result_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_result *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_result, ce);

  PHP5TO7_ZVAL_UNDEF(self->value);

  PHP5TO7_ZEND_OBJECT_INIT(dse_graph_result, self, ce);
}

void cassandra_define_DseGraphResult(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Result", dse_graph_result_methods);
  dse_graph_result_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_result_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
  dse_graph_result_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_result_ce->create_object = php_dse_graph_result_new;

  memcpy(&dse_graph_result_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_result_handlers.get_properties  = php_dse_graph_result_properties;
  dse_graph_result_handlers.compare_objects = php_dse_graph_result_compare;
  dse_graph_result_handlers.clone_obj = NULL;
}
