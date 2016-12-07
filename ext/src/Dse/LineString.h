#ifndef PHP_DSE_LINE_STRING_H
#define PHP_DSE_LINE_STRING_H

char *line_string_to_wkt(dse_line_string *line_string TSRMLS_DC);
char *line_string_to_string(dse_line_string *line_string TSRMLS_DC);

int php_dse_line_string_construct_from_iterator(DseLineStringIterator* iterator,
                                                zval *return_value TSRMLS_DC);

#endif
