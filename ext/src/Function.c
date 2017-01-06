/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_function_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_function_methods[] = {
  PHP_ABSTRACT_ME(Function, name, arginfo_none)
  PHP_ABSTRACT_ME(Function, simpleName, arginfo_none)
  PHP_ABSTRACT_ME(Function, arguments, arginfo_none)
  PHP_ABSTRACT_ME(Function, returnType, arginfo_none)
  PHP_ABSTRACT_ME(Function, signature, arginfo_none)
  PHP_ABSTRACT_ME(Function, language, arginfo_none)
  PHP_ABSTRACT_ME(Function, body, arginfo_none)
  PHP_ABSTRACT_ME(Function, isCalledOnNullInput, arginfo_none)
  PHP_FE_END
};

void php_driver_define_Function(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Function", php_driver_function_methods);
  php_driver_function_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_function_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
