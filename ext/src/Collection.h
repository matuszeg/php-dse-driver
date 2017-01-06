/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_COLLECTION_H
#define PHP_DRIVER_COLLECTION_H

int php_driver_collection_add(php_driver_collection* collection, zval* object TSRMLS_DC);

#endif /* PHP_DRIVER_COLLECTION_H */
