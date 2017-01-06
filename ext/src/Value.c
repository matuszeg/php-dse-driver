/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry* php_driver_value_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_value_methods[] = {
  PHP_ABSTRACT_ME(Value, type, arginfo_none)
  PHP_FE_END
};

void
php_driver_define_Value(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Value", php_driver_value_methods);
  php_driver_value_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_value_ce->ce_flags |= ZEND_ACC_INTERFACE;
}

