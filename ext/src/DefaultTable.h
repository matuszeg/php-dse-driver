/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_DEFAULT_TABLE_H
#define PHP_DRIVER_DEFAULT_TABLE_H

#include "php_driver.h"

php5to7_zval
php_driver_create_table(php_driver_ref* schema,
                           const CassTableMeta *meta TSRMLS_DC);

#endif /* PHP_DRIVER_DEFAULT_TABLE_H */
