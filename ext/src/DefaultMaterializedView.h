/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_DEFAULT_MATERIALIZED_VIEW_H
#define PHP_DRIVER_DEFAULT_MATERIALIZED_VIEW_H

#include "php_driver.h"

php5to7_zval
php_driver_create_materialized_view(php_driver_ref* schema,
                                       const CassMaterializedViewMeta *meta TSRMLS_DC);

#endif /* PHP_DRIVER_DEFAULT_MATERIALIZED_VIEW_H */
