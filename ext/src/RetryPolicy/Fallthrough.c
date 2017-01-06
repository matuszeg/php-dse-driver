/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/types.h"

zend_class_entry *php_driver_retry_policy_fallthrough_ce = NULL;

static zend_function_entry php_driver_retry_policy_fallthrough_methods[] = {
  PHP_FE_END
};

static zend_object_handlers php_driver_retry_policy_fallthrough_handlers;

static void
php_driver_retry_policy_fallthrough_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_retry_policy *self = PHP5TO7_ZEND_OBJECT_GET(retry_policy, object);

  cass_retry_policy_free(self->policy);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_retry_policy_fallthrough_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_retry_policy *self = PHP5TO7_ZEND_OBJECT_ECALLOC(retry_policy, ce);

  self->policy = cass_retry_policy_fallthrough_new();

  PHP5TO7_ZEND_OBJECT_INIT_EX(retry_policy, retry_policy_fallthrough, self, ce);
}

void php_driver_define_RetryPolicyFallthrough(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\RetryPolicy\\Fallthrough", php_driver_retry_policy_fallthrough_methods);
  php_driver_retry_policy_fallthrough_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_retry_policy_fallthrough_ce TSRMLS_CC, 1, php_driver_retry_policy_ce);
  php_driver_retry_policy_fallthrough_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_retry_policy_fallthrough_ce->create_object = php_driver_retry_policy_fallthrough_new;

  memcpy(&php_driver_retry_policy_fallthrough_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
