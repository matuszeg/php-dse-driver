#ifndef PHP_DSE_POLYGON_H
#define PHP_DSE_POLYGON_H

char *polygon_to_wkt(dse_polygon *polygon TSRMLS_DC);
char *polygon_to_string(dse_polygon *polygon TSRMLS_DC);

int php_dse_polygon_construct_from_iterator(DsePolygonIterator* iterator,
                                            zval *return_value TSRMLS_DC);

#endif
