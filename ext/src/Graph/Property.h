/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_GRAPH_DEFAULT_PROPERTY_H
#define PHP_DRIVER_GRAPH_DEFAULT_PROPERTY_H

void
php_driver_graph_property_init(php_driver_graph_property_base * property);

void
php_driver_graph_property_destroy(php_driver_graph_property_base * property);

void
php_driver_graph_property_properties(php_driver_graph_property_base* property,
                                     HashTable *props TSRMLS_DC);

#endif /* PHP_DRIVER_GRAPH_DEFAULT_PROPERTY_H */
