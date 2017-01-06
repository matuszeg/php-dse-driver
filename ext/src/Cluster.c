/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_cluster_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_keyspace, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_cluster_methods[] = {
  PHP_ABSTRACT_ME(Cluster, connect, arginfo_keyspace)
  PHP_ABSTRACT_ME(Cluster, connectAsync, arginfo_keyspace)
  PHP_FE_END
};

void php_driver_define_Cluster(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Cluster", php_driver_cluster_methods);
  php_driver_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_cluster_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
