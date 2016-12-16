#ifndef PHP_DRIVER_GRAPH_ELEMENT_H
#define PHP_DRIVER_GRAPH_ELEMENT_H

void
php_driver_graph_element_init(php_driver_graph_element_base * element);

void
php_driver_graph_element_destroy(php_driver_graph_element_base * element);

int
php_driver_graph_element_header_populate(HashTable *ht,
                                         php_driver_graph_element_base* element TSRMLS_DC);

void
php_driver_graph_element_properties(php_driver_graph_element_base* element,
                                    HashTable *props TSRMLS_DC);


#endif /* PHP_DRIVER_GRAPH_ELEMENT_H */
