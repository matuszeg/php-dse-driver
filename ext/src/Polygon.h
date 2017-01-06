/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_POLYGON_H
#define PHP_DRIVER_POLYGON_H

#define DSE_POLYGON_TYPE "org.apache.cassandra.db.marshal.PolygonType"

int php_driver_polygon_bind_by_index(CassStatement *statement,
                                     size_t index,
                                     zval *value TSRMLS_DC);

int php_driver_polygon_bind_by_name(CassStatement *statement,
                                    const char *name,
                                    zval *value TSRMLS_DC);

int php_driver_polygon_construct_from_value(const CassValue *value,
                                            php5to7_zval *out TSRMLS_DC);

char *php_driver_polygon_to_wkt(php_driver_polygon *polygon TSRMLS_DC);

char *php_driver_polygon_to_string(php_driver_polygon *polygon TSRMLS_DC);

int php_driver_polygon_construct_from_iterator(DsePolygonIterator* iterator,
                                               zval *return_value TSRMLS_DC);

#endif
