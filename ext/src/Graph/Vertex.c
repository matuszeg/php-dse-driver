#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_vertex_ce = NULL;

static zend_function_entry php_driver_graph_vertex_methods[] = {
  PHP_FE_END
};

void php_driver_define_GraphVertex(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Vertex", php_driver_graph_vertex_methods);
  php_driver_graph_vertex_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_vertex_ce TSRMLS_CC, 1, php_driver_graph_element_ce);
  php_driver_graph_vertex_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
