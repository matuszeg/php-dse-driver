/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_DATE_RANGE_H
#define PHP_DRIVER_DATE_RANGE_H

PHP_DRIVER_DECLARE_DSE_TYPE_HELPERS(date_range)

int php_driver_date_range_construct_from_value(const CassValue *value,
                                               php5to7_zval *out TSRMLS_DC);

#endif
