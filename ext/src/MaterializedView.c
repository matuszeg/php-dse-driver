/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_materialized_view_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_materialized_view_methods[] = {
  PHP_ABSTRACT_ME(MaterializedView, baseTable, arginfo_none)
  PHP_FE_END
};

void php_driver_define_MaterializedView(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\MaterializedView", php_driver_materialized_view_methods);
  php_driver_materialized_view_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_materialized_view_ce TSRMLS_CC, 1, php_driver_table_ce);
  php_driver_materialized_view_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
