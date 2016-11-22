#ifndef PHP_DSE_GRAPH_OPTIONS_H
#define PHP_DSE_GRAPH_OPTIONS_H

void
php_dse_graph_options_init(dse_graph_options *options);

void
php_dse_graph_options_destroy(dse_graph_options *options);

void
php_dse_graph_options_clone(dse_graph_options *dst, dse_graph_options *src);

#endif /* PHP_DSE_GRAPH_OPTIONS_H */
