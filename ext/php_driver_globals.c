#include "php_driver.h"
#include "php_driver_globals.h"

DseLineString *php_driver_line_string_g(TSRMLS_D) {
  dse_line_string_reset(PHP_DRIVER_G(line_string));
  return PHP_DRIVER_G(line_string);
}

DsePolygon *php_driver_polygon_g(TSRMLS_D) {
  dse_polygon_reset(PHP_DRIVER_G(polygon));
  return PHP_DRIVER_G(polygon);
}
