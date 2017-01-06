/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_CONSISTENCY_H
#define PHP_DRIVER_CONSISTENCY_H

int php_driver_get_consistency(zval *consistency, long *result TSRMLS_DC);
int php_driver_get_serial_consistency(zval *serial_consistency, long *result TSRMLS_DC);

#endif /* PHP_DRIVER_CONSISTENCY_H */
