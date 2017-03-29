/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_GLOBALS_H
#define PHP_DRIVER_GLOBALS_H

#include "php_driver_types.h"

ZEND_BEGIN_MODULE_GLOBALS(php_driver)
  CassUuidGen           *uuid_gen;
  pid_t                  uuid_gen_pid;
  unsigned int           persistent_clusters;
  unsigned int           persistent_sessions;

#define XX_SCALAR(name, _) php5to7_zval type_##name;

  PHP_DRIVER_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR

#define XX_DSE_TYPE(name, _, __, ___) php5to7_zval type_##name;

  PHP_DRIVER_DSE_TYPES_MAP(XX_DSE_TYPE)
#undef XX_DSE_TYPE

  DseLineString         *line_string; // Don't use directly use php_driver_line_string_g()
  DseLineStringIterator *iterator_line_string;
  DsePolygon            *polygon; // Don't use directly use php_driver_polygon_g()
  DsePolygonIterator    *iterator_polygon;
ZEND_END_MODULE_GLOBALS(php_driver)

DseLineString *php_driver_line_string_g(TSRMLS_D);
DsePolygon *php_driver_polygon_g(TSRMLS_D);

ZEND_EXTERN_MODULE_GLOBALS(php_driver)

#endif /* PHP_DRIVER_GLOBALS_H */
