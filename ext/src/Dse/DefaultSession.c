#include "php_dse.h"
#include "php_dse_types.h"

#include "Cassandra/Session.h"

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

  php_cassandra_session_execute(&self->base, statement, options, return_value);
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

  php_cassandra_session_execute_async(&self->base, statement, options, return_value);
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

  php_cassandra_session_prepare(&self->base, cql, options, return_value);
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

  php_cassandra_session_prepare_async(&self->base, cql, options, return_value);
}

PHP_METHOD(DseDefaultSession, close)
{
  zval *timeout = NULL;

  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  php_cassandra_session_close(&self->base, timeout, return_value);
}

PHP_METHOD(DseDefaultSession, closeAsync)
{
  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  php_cassandra_session_close_async(&self->base, return_value);
}

PHP_METHOD(DseDefaultSession, schema)
{
  dse_session *self = PHP_DSE_GET_SESSION(getThis());

  if (zend_parse_parameters_none() == FAILURE)
    return;

  php_cassandra_session_schema(&self->base, return_value);
}

PHP_METHOD(DseDefaultSession, executeGraph)
{
  zval *statement = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

}

PHP_METHOD(DseDefaultSession, executeGraphAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  dse_session *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_SESSION(getThis());

}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, statement, Cassandra\\Statement, 0)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
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
  PHP_ME(DseDefaultSession, executeGraph, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultSession, executeGraphAsync, arginfo_execute, ZEND_ACC_PUBLIC)
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

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_default_session_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_session *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_session, ce);

  php_cassandra_session_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_session, dse_default_session, self, ce);
}

void dse_define_DefaultSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\DefaultSession", dse_default_session_methods);
  dse_default_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_default_session_ce TSRMLS_CC, 1, dse_session_ce);
  dse_default_session_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_default_session_ce->create_object = php_dse_default_session_new;

  memcpy(&dse_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_default_session_handlers.get_properties  = php_dse_default_session_properties;
  dse_default_session_handlers.compare_objects = php_dse_default_session_compare;
  dse_default_session_handlers.clone_obj = NULL;
}
