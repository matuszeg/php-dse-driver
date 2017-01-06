/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry* php_driver_custom_ce = NULL;

static zend_function_entry php_driver_custom_methods[] = {
  PHP_FE_END
};

void
php_driver_define_Custom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Custom", php_driver_custom_methods);
  php_driver_custom_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_custom_ce TSRMLS_CC, 1, php_driver_value_ce);
  php_driver_custom_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
