#ifndef PHP_DSE_GLOBALS_H
#define PHP_DSE_GLOBALS_H

#define PHP_DRIVER_EXTRA_MODULE_GLOBALS \
  php5to7_zval  type_line_string;       \
  php5to7_zval  type_point;             \
  php5to7_zval  type_polygon;

#include "php_driver_globals.h"

ZEND_EXTERN_MODULE_GLOBALS(php_driver)

#define DSE_G(v) PHP_DRIVER_G(v)

#endif /* PHP_DSE_GLOBALS_H */
