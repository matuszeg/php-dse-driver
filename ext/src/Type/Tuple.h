/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_TYPE_TUPLE_H
#define PHP_DRIVER_TYPE_TUPLE_H

int php_driver_type_tuple_add(php_driver_type *type,
                                 zval *zsub_type TSRMLS_DC);

#endif /* PHP_DRIVER_TYPE_TUPLE_H */

