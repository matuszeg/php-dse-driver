#ifndef PHP_DSE_GRAPH_DEFAULT_PROPERTY_H
#define PHP_DSE_GRAPH_DEFAULT_PROPERTY_H

void
php_dse_graph_property_init(dse_graph_property_base * property);

void
php_dse_graph_property_destroy(dse_graph_property_base * property);

void
php_dse_graph_property_properties(dse_graph_property_base* property,
                                  HashTable *props TSRMLS_DC);

#endif /* PHP_DSE_GRAPH_DEFAULT_PROPERTY_H */
