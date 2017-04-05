/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_DATE_RANGE_BOUND_H
#define PHP_DRIVER_DATE_RANGE_BOUND_H

int php_driver_validate_bound_precision(long precision);
int php_driver_bound_get_time_from_zval(zval* time_ms_zval, const char* param_name, cass_int64_t* time_ms TSRMLS_DC);

#endif
