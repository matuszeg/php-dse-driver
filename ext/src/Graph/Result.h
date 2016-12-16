#ifndef PHP_DRIVER_GRAPH_RESULT_H
#define PHP_DRIVER_GRAPH_RESULT_H

int
php_driver_graph_result_construct(const DseGraphResult *result,
                                  zval *return_value TSRMLS_DC);

#endif /* PHP_DRIVER_GRAPH_RESULT_H */
