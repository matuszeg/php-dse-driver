/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/types.h"

zend_class_entry *php_driver_retry_policy_ce = NULL;

static zend_function_entry php_driver_retry_policy_methods[] = {
  PHP_FE_END
};

void php_driver_define_RetryPolicy(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\RetryPolicy", php_driver_retry_policy_methods);
  php_driver_retry_policy_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_retry_policy_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
