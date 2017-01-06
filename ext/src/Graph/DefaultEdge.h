/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_GRAPH_DEFAULT_EDGE_H
#define PHP_DRIVER_GRAPH_DEFAULT_EDGE_H

int
php_driver_graph_default_edge_construct(HashTable *ht,
                                        zval *return_value TSRMLS_DC);

#endif /* PHP_DRIVER_GRAPH_DEFAULT_EDGE_H */
