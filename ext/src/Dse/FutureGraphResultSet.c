#include "php_dse.h"
#include "php_dse_types.h"
#include "util/future.h"
#include "util/ref.h"
#include "GraphResultSet.h"

zend_class_entry *dse_future_graph_result_set_ce = NULL;

PHP_METHOD(DseFutureGraphResultSet, get)
{
  zval *timeout = NULL;
  dse_future_graph_result_set *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_FUTURE_GRAPH_RESULT_SET(getThis());

  if (PHP5TO7_ZVAL_IS_UNDEF(self->result_set)) { /* If not set then wait for the result */
    if (php_cassandra_future_wait_timed(self->future, timeout TSRMLS_CC) == FAILURE ||
        php_cassandra_future_is_error(self->future TSRMLS_CC) == FAILURE) {
      return;
    }

    if (php_dse_graph_result_set_build(self->future, PHP5TO7_ZVAL_MAYBE_P(self->result_set) TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->result_set), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry dse_future_graph_result_set_methods[] = {
  PHP_ME(DseFutureGraphResultSet, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_future_graph_result_set_handlers;

static HashTable *
php_dse_future_graph_result_set_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_future_graph_result_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_future_graph_result_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_future_graph_result_set *self =
      PHP5TO7_ZEND_OBJECT_GET(dse_future_graph_result_set, object);

  if (self->future) {
    cass_future_free(self->future);
  }

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->result_set);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_future_graph_result_set_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_future_graph_result_set *self
      = PHP5TO7_ZEND_OBJECT_ECALLOC(dse_future_graph_result_set, ce);

  self->future = NULL;

  PHP5TO7_ZVAL_UNDEF(self->result_set);

  PHP5TO7_ZEND_OBJECT_INIT(dse_future_graph_result_set, self, ce);
}

void dse_define_FutureGraphResultSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\FutureGraphResultSet", dse_future_graph_result_set_methods);
  dse_future_graph_result_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_future_graph_result_set_ce TSRMLS_CC, 1, cassandra_future_ce);
  dse_future_graph_result_set_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_future_graph_result_set_ce->create_object = php_dse_future_graph_result_set_new;

  memcpy(&dse_future_graph_result_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_future_graph_result_set_handlers.get_properties  = php_dse_future_graph_result_set_properties;
  dse_future_graph_result_set_handlers.compare_objects = php_dse_future_graph_result_set_compare;
  dse_future_graph_result_set_handlers.clone_obj = NULL;
}
