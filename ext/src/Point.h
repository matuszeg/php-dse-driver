/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_POINT_H
#define PHP_DRIVER_POINT_H

PHP_DRIVER_DECLARE_DSE_TYPE_HELPERS(point)

char *php_driver_point_to_wkt(php_driver_point *point);

char *php_driver_point_to_string(php_driver_point *point);

#endif
