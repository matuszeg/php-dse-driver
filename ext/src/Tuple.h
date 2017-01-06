/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_TUPLE_H
#define PHP_DRIVER_TUPLE_H

int php_driver_tuple_set(php_driver_tuple *tuple, ulong index, zval *object TSRMLS_DC);

#endif /* PHP_DRIVER_TUPLE_H */
