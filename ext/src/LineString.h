/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_LINE_STRING_H
#define PHP_DRIVER_LINE_STRING_H

PHP_DRIVER_DECLARE_DSE_TYPE_HELPERS(line_string)

int php_driver_line_string_construct_from_value(const CassValue *value,
                                                php5to7_zval *out TSRMLS_DC);

char *php_driver_line_string_to_wkt(php_driver_line_string *line_string TSRMLS_DC);

char *php_driver_line_string_to_string(php_driver_line_string *line_string TSRMLS_DC);

int php_driver_line_string_construct_from_iterator(DseLineStringIterator* iterator,
                                                   zval *return_value TSRMLS_DC);

#endif
