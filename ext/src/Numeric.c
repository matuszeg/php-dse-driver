/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_numeric_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_num, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_numeric_methods[] = {
  PHP_ABSTRACT_ME(Numeric, add, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, sub, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, mul, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, div, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, mod, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, abs, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, neg, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, sqrt, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, toInt, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, toDouble, arginfo_none)
  PHP_FE_END
};

void
php_driver_define_Numeric(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Numeric", php_driver_numeric_methods);
  php_driver_numeric_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_numeric_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
