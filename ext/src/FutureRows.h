/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_FUTURE_ROWS_H
#define PHP_DRIVER_FUTURE_ROWS_H

int
php_driver_future_rows_get_result(php_driver_future_rows *future_rows, zval *timeout TSRMLS_DC);

#endif /* PHP_DRIVER_FUTURE_ROWS_H */
