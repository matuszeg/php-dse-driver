/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_index_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_index_methods[] = {
  PHP_ABSTRACT_ME(Index, name, arginfo_none)
  PHP_ABSTRACT_ME(Index, kind, arginfo_none)
  PHP_ABSTRACT_ME(Index, target, arginfo_none)
  PHP_ABSTRACT_ME(Index, option, arginfo_name)
  PHP_ABSTRACT_ME(Index, options, arginfo_none)
  PHP_ABSTRACT_ME(Index, className, arginfo_none)
  PHP_ABSTRACT_ME(Index, isCustom, arginfo_none)
  PHP_FE_END
};

void php_driver_define_Index(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Index", php_driver_index_methods);
  php_driver_index_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_index_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
