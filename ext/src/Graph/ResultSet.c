#include "php_driver.h"
#include "php_driver_types.h"

#include "Result.h"
#include "ResultSet.h"

zend_class_entry *php_driver_graph_result_set_ce = NULL;

int
php_driver_graph_result_set_build(CassFuture *future, zval *return_value TSRMLS_DC)
{
  size_t i, count;
  php_driver_graph_result_set *result_set = NULL;

  DseGraphResultSet *graph_result_set = cass_future_get_dse_graph_resultset(future);
  if (!graph_result_set) {
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                            "Future doesn't contain a graph result set.");
    return FAILURE;
  }

  object_init_ex(return_value, php_driver_graph_result_set_ce);
  result_set = PHP_DRIVER_GET_GRAPH_RESULT_SET(return_value);

  count = dse_graph_resultset_count(graph_result_set);
  for (i = 0; i < count; ++i) {
    php5to7_zval result;
    const DseGraphResult *graph_result = dse_graph_resultset_next(graph_result_set);

    if (!graph_result) {
      zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                              "Result set doesn't contain a result at the current position.");
      return FAILURE;
    }

    PHP5TO7_ZVAL_MAYBE_MAKE(result);
    if (php_driver_graph_result_construct(graph_result,
                                          PHP5TO7_ZVAL_MAYBE_P(result) TSRMLS_CC) == FAILURE) {
      PHP5TO7_ZVAL_MAYBE_DESTROY(result);
      return FAILURE;
    }

    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(PHP5TO7_Z_ARRVAL_MAYBE_P(result_set->results),
                                        PHP5TO7_ZVAL_MAYBE_P(result), sizeof(zval *));
  }

  return SUCCESS;
}

PHP_METHOD(GraphResultSet, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\ResultSet objects directly is not supported."
                          );
  return;
}

PHP_METHOD(GraphResultSet, count)
{
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  RETURN_LONG(zend_hash_num_elements(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results)));
}

PHP_METHOD(GraphResultSet, rewind)
{
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  zend_hash_internal_pointer_reset(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results));
}

PHP_METHOD(GraphResultSet, current)
{
  php5to7_zval *entry;
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results), entry)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(entry), 1, 0);
  }
}

PHP_METHOD(GraphResultSet, key)
{
  php5to7_ulong num_index;
  php5to7_string str_index;
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_KEY(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results),
                                        &str_index, &num_index) == HASH_KEY_IS_LONG)
    RETURN_LONG(num_index);
}

PHP_METHOD(GraphResultSet, next)
{
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  zend_hash_move_forward(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results));
}

PHP_METHOD(GraphResultSet, valid)
{
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  RETURN_BOOL(zend_hash_has_more_elements(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results)) == SUCCESS);
}

PHP_METHOD(GraphResultSet, offsetExists)
{
  zval *offset;
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "a positive integer");
  }

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  RETURN_BOOL(zend_hash_index_exists(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results), (php5to7_ulong) Z_LVAL_P(offset)));
}

PHP_METHOD(GraphResultSet, offsetGet)
{
  zval *offset;
  php5to7_zval *value;
  php_driver_graph_result_set *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "a positive integer");
  }

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());
  if (PHP5TO7_ZEND_HASH_INDEX_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results), Z_LVAL_P(offset), value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
  }

  RETURN_FALSE;
}

PHP_METHOD(GraphResultSet, offsetSet)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                          "Cannot overwrite a row at a given offset, rows are immutable."
                          );
  return;
}

PHP_METHOD(GraphResultSet, offsetUnset)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(php_driver_domain_exception_ce, 0 TSRMLS_CC,
                          "Cannot delete a row at a given offset, rows are immutable."
                          );
  return;
}

PHP_METHOD(GraphResultSet, first)
{
  HashPosition pos;
  php5to7_zval *entry;
  php_driver_graph_result_set* self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_RESULT_SET(getThis());

  zend_hash_internal_pointer_reset_ex(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results), &pos);
  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(PHP5TO7_Z_ARRVAL_MAYBE_P(self->results), entry)) {
    RETVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(entry), 1, 0);
  }
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

static zend_function_entry php_driver_graph_result_set_methods[] = {
  PHP_ME(GraphResultSet, __construct,  arginfo_none,    ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Iterator */
  PHP_ME(GraphResultSet, count,        arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, rewind,       arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, current,      arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, key,          arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, next,         arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, valid,        arginfo_none,    ZEND_ACC_PUBLIC)
  /* Array access */
  PHP_ME(GraphResultSet, offsetExists, arginfo_offset,  ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, offsetGet,    arginfo_offset,  ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, offsetSet,    arginfo_set,     ZEND_ACC_PUBLIC)
  PHP_ME(GraphResultSet, offsetUnset,  arginfo_offset,  ZEND_ACC_PUBLIC)
  /* Graph result set */
  PHP_ME(GraphResultSet, first,        arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_result_set_handlers;

static HashTable *
php_driver_graph_result_set_properties(zval *object TSRMLS_DC)
{
  php5to7_zval value;
  php_driver_graph_result_set *self  = PHP_DRIVER_GET_GRAPH_RESULT_SET(object);
  HashTable                   *props = zend_std_get_properties(object TSRMLS_CC);

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->results), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "results", sizeof("results"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  return props;
}

static int
php_driver_graph_result_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_result_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_result_set *self = PHP5TO7_ZEND_OBJECT_GET(graph_result_set, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->results);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_result_set_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_result_set *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_result_set, ce);

  PHP5TO7_ZVAL_MAYBE_MAKE(self->results);
  array_init(PHP5TO7_ZVAL_MAYBE_P(self->results));


  PHP5TO7_ZEND_OBJECT_INIT(graph_result_set, self, ce);
}

void php_driver_define_GraphResultSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\ResultSet", php_driver_graph_result_set_methods);
  php_driver_graph_result_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_result_set_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
  php_driver_graph_result_set_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_result_set_ce->create_object = php_driver_graph_result_set_new;

  memcpy(&php_driver_graph_result_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_result_set_handlers.get_properties  = php_driver_graph_result_set_properties;
  php_driver_graph_result_set_handlers.compare_objects = php_driver_graph_result_set_compare;
  php_driver_graph_result_set_handlers.clone_obj = NULL;
}
