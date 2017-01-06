/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/future.h"
#include "util/ref.h"

#include "ResultSet.h"

zend_class_entry *php_driver_graph_future_result_set_ce = NULL;

PHP_METHOD(GraphFutureResultSet, get)
{
  zval *timeout = NULL;
  php_driver_graph_future_result_set *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_FUTURE_RESULT_SET(getThis());

  if (PHP5TO7_ZVAL_IS_UNDEF(self->result_set)) { /* If not set then wait for the result */
    if (php_driver_future_wait_timed(self->future, timeout TSRMLS_CC) == FAILURE ||
        php_driver_future_is_error(self->future TSRMLS_CC) == FAILURE) {
      return;
    }

    PHP5TO7_ZVAL_MAYBE_MAKE(PHP5TO7_ZVAL_MAYBE_P(self->result_set));
    if (php_driver_graph_result_set_build(self->future, PHP5TO7_ZVAL_MAYBE_P(self->result_set) TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->result_set), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_future_result_set_methods[] = {
  PHP_ME(GraphFutureResultSet, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_future_result_set_handlers;

static HashTable *
php_driver_graph_future_result_set_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_graph_future_result_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_future_result_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_future_result_set *self =
      PHP5TO7_ZEND_OBJECT_GET(graph_future_result_set, object);

  if (self->future) {
    cass_future_free(self->future);
  }

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->result_set);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_future_result_set_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_future_result_set *self
      = PHP5TO7_ZEND_OBJECT_ECALLOC(graph_future_result_set, ce);

  self->future = NULL;

  PHP5TO7_ZVAL_UNDEF(self->result_set);

  PHP5TO7_ZEND_OBJECT_INIT(graph_future_result_set, self, ce);
}

void php_driver_define_GraphFutureResultSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\FutureResultSet", php_driver_graph_future_result_set_methods);
  php_driver_graph_future_result_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_future_result_set_ce TSRMLS_CC, 1, php_driver_future_ce);
  php_driver_graph_future_result_set_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_future_result_set_ce->create_object = php_driver_graph_future_result_set_new;

  memcpy(&php_driver_graph_future_result_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_future_result_set_handlers.get_properties  = php_driver_graph_future_result_set_properties;
  php_driver_graph_future_result_set_handlers.compare_objects = php_driver_graph_future_result_set_compare;
  php_driver_graph_future_result_set_handlers.clone_obj = NULL;
}
