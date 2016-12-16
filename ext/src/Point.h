#ifndef PHP_DRIVER_POINT_H
#define PHP_DRIVER_POINT_H

#define DSE_POINT_TYPE "org.apache.cassandra.db.marshal.PointType"

int php_driver_point_bind_by_index(CassStatement *statement,
                                   size_t index,
                                   zval *value TSRMLS_DC);

int php_driver_point_bind_by_name(CassStatement *statement,
                                  const char *name,
                                  zval *value TSRMLS_DC);

int php_driver_point_construct_from_value(const CassValue *value,
                                          php5to7_zval *out TSRMLS_DC);

char *php_driver_point_to_wkt(php_driver_point *point);

char *php_driver_point_to_string(php_driver_point *point);

#endif
