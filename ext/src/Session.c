/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_session_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  PHP_DRIVER_NAMESPACE_ZEND_ARG_OBJ_INFO(0, options, ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  PHP_DRIVER_NAMESPACE_ZEND_ARG_OBJ_INFO(0, options, ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute_graph, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_session_methods[] = {
  PHP_ABSTRACT_ME(Session, execute, arginfo_execute)
  PHP_ABSTRACT_ME(Session, executeAsync, arginfo_execute)
  PHP_ABSTRACT_ME(Session, prepare, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, prepareAsync, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, close, arginfo_timeout)
  PHP_ABSTRACT_ME(Session, closeAsync, arginfo_none)
  PHP_ABSTRACT_ME(Session, schema, arginfo_none)
  PHP_ABSTRACT_ME(Session, executeGraph, arginfo_execute_graph)
  PHP_ABSTRACT_ME(Session, executeGraphAsync, arginfo_execute_graph)
  PHP_FE_END
};

void php_driver_define_Session(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Session", php_driver_session_methods);
  php_driver_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_session_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
