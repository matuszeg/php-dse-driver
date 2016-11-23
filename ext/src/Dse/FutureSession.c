#include "php_dse.h"
#include "php_dse_types.h"
#include "util/future.h"
#include "util/ref.h"

#include "Cassandra/FutureSession.h"

zend_class_entry *dse_future_session_ce = NULL;

PHP_METHOD(DseFutureSession, get)
{
  zval *timeout = NULL;
  cassandra_session_base *session = NULL;
  cassandra_future_session_base *future = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  future = &PHP_DSE_GET_FUTURE_SESSION(getThis())->base;

  if (!PHP5TO7_ZVAL_IS_UNDEF(future->default_session)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(future->default_session), 1, 0);
  }

  object_init_ex(return_value, dse_default_session_ce);
  session = &PHP_DSE_GET_SESSION(return_value)->base;

  php_cassandra_future_session_get(future, timeout, session TSRMLS_CC);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(future->default_session),
                    return_value);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry dse_future_session_methods[] = {
  PHP_ME(DseFutureSession, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_future_session_handlers;

static HashTable *
php_dse_future_session_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_future_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_future_session_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_future_session *self =
      PHP5TO7_ZEND_OBJECT_GET(dse_future_session, object);

  php_cassandra_future_session_destroy(&self->base);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_future_session_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_future_session *self
      = PHP5TO7_ZEND_OBJECT_ECALLOC(dse_future_session, ce);

  php_cassandra_future_session_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT(dse_future_session, self, ce);
}

void dse_define_FutureSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\FutureSession", dse_future_session_methods);
  dse_future_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_future_session_ce TSRMLS_CC, 1, cassandra_future_ce);
  dse_future_session_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_future_session_ce->create_object = php_dse_future_session_new;

  memcpy(&dse_future_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_future_session_handlers.get_properties  = php_dse_future_session_properties;
  dse_future_session_handlers.compare_objects = php_dse_future_session_compare;
  dse_future_session_handlers.clone_obj = NULL;
}
