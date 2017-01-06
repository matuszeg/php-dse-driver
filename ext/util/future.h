/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_UTIL_FUTURE_H
#define PHP_DRIVER_UTIL_FUTURE_H

int  php_driver_future_wait_timed(CassFuture *future, zval *timeout TSRMLS_DC);
int  php_driver_future_is_error(CassFuture *future TSRMLS_DC);

#endif /* PHP_DRIVER_UTIL_FUTURE_H */
