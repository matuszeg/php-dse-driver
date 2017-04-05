/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

zend_class_entry *php_driver_date_range_precision_ce = NULL;

void php_driver_define_DateRangePrecision(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DateRange\\Precision", NULL);
  php_driver_date_range_precision_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_date_range_precision_ce->ce_flags     |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

#define SET_CONSTANT(NAME) php5to7_zend_declare_class_constant_long(php_driver_date_range_precision_ce, \
      #NAME, sizeof(#NAME) - 1, DSE_DATE_RANGE_PRECISION_##NAME)
  SET_CONSTANT(UNBOUNDED);
  SET_CONSTANT(YEAR);
  SET_CONSTANT(MONTH);
  SET_CONSTANT(DAY);
  SET_CONSTANT(HOUR);
  SET_CONSTANT(MINUTE);
  SET_CONSTANT(SECOND);
  SET_CONSTANT(MILLISECOND);
#undef SET_CONSTANT
}
