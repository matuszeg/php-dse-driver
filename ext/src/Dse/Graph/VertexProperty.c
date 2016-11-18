#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_vertex_property_ce = NULL;

static zend_function_entry dse_graph_vertex_property_methods[] = {
  PHP_FE_END
};

void dse_define_GraphVertexProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\VertexProperty", dse_graph_vertex_property_methods);
  dse_graph_vertex_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_vertex_property_ce TSRMLS_CC, 2, dse_graph_property_ce, dse_graph_element_ce);
  dse_graph_vertex_property_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
