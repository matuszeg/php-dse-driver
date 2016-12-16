#ifndef PHP_DRIVER_LINE_STRING_H
#define PHP_DRIVER_LINE_STRING_H

#define DSE_LINE_STRING_TYPE "org.apache.cassandra.db.marshal.LineStringType"

int php_driver_line_string_bind_by_index(CassStatement *statement,
                                         size_t index,
                                         zval *value TSRMLS_DC);

int php_driver_line_string_bind_by_name(CassStatement *statement,
                                        const char *name,
                                        zval *value TSRMLS_DC);

int php_driver_line_string_construct_from_value(const CassValue *value,
                                                php5to7_zval *out TSRMLS_DC);

char *php_driver_line_string_to_wkt(php_driver_line_string *line_string TSRMLS_DC);

char *php_driver_line_string_to_string(php_driver_line_string *line_string TSRMLS_DC);

int php_driver_line_string_construct_from_iterator(DseLineStringIterator* iterator,
                                                   zval *return_value TSRMLS_DC);

#endif
