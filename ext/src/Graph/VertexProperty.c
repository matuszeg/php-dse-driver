#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_vertex_property_ce = NULL;

static zend_function_entry php_driver_graph_vertex_property_methods[] = {
  PHP_FE_END
};

void php_driver_define_GraphVertexProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\VertexProperty", php_driver_graph_vertex_property_methods);
  php_driver_graph_vertex_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_vertex_property_ce TSRMLS_CC, 2, php_driver_graph_property_ce, php_driver_graph_element_ce);
  php_driver_graph_vertex_property_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
