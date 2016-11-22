#ifndef PHP_DSE_GRAPH_ELEMENT_H
#define PHP_DSE_GRAPH_ELEMENT_H

void
php_dse_graph_element_init(dse_graph_element_base * element);

void
php_dse_graph_element_destroy(dse_graph_element_base * element);

int
php_dse_graph_element_header_populate(HashTable *ht,
                                      dse_graph_element_base* element TSRMLS_DC);

void
php_dse_graph_element_properties(dse_graph_element_base* element,
                                 HashTable *props TSRMLS_DC);


#endif /* PHP_DSE_GRAPH_ELEMENT_H */
