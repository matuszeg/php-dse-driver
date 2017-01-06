/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_keyspace_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_signature, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_keyspace_methods[] = {
  PHP_ABSTRACT_ME(Keyspace, name, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, replicationClassName, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, replicationOptions, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, hasDurableWrites, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, table, arginfo_name)
  PHP_ABSTRACT_ME(Keyspace, tables, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, userType, arginfo_name)
  PHP_ABSTRACT_ME(Keyspace, userTypes, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, materializedView, arginfo_name)
  PHP_ABSTRACT_ME(Keyspace, materializedViews, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, function, arginfo_signature)
  PHP_ABSTRACT_ME(Keyspace, functions, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, aggregate, arginfo_signature)
  PHP_ABSTRACT_ME(Keyspace, aggregates, arginfo_none)
  PHP_FE_END
};

void php_driver_define_Keyspace(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Keyspace", php_driver_keyspace_methods);
  php_driver_keyspace_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_keyspace_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
