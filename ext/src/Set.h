/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_SET_H
#define PHP_DRIVER_SET_H

int php_driver_set_add(php_driver_set* set, zval* object TSRMLS_DC);

#endif /* PHP_DRIVER_SET_H */
