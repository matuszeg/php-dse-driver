/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_aggregate_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_aggregate_methods[] = {
  PHP_ABSTRACT_ME(Aggregate, name, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, simpleName, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, argumentTypes, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, finalFunction, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, stateFunction, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, initialCondition, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, returnType, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, stateType, arginfo_none)
  PHP_ABSTRACT_ME(Aggregate, signature, arginfo_none)
  PHP_FE_END
};

void php_driver_define_Aggregate(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Aggregate", php_driver_aggregate_methods);
  php_driver_aggregate_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_aggregate_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
