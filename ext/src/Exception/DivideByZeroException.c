/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_divide_by_zero_exception_ce = NULL;

static zend_function_entry DivideByZeroException_methods[] = {
  PHP_FE_END
};

void php_driver_define_DivideByZeroException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Exception\\DivideByZeroException", DivideByZeroException_methods);
  php_driver_divide_by_zero_exception_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_range_exception_ce);
  zend_class_implements(php_driver_divide_by_zero_exception_ce TSRMLS_CC, 1, php_driver_exception_ce);
}
