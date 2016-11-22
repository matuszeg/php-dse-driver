#ifndef PHP_DSE_GRAPH_DEFAULT_ELEMENT_H
#define PHP_DSE_GRAPH_DEFAULT_ELEMENT_H

int
php_dse_graph_default_element_populate(HashTable *ht,
                                       zval *element,
                                       dse_graph_element_base* element_base TSRMLS_DC);

#endif /* PHP_DSE_GRAPH_DEFAULT_ELEMENT_H */
