#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_vertex_ce = NULL;

static zend_function_entry dse_graph_vertex_methods[] = {
  PHP_FE_END
};

void dse_define_GraphVertex(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Vertex", dse_graph_vertex_methods);
  dse_graph_vertex_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_vertex_ce TSRMLS_CC, 1, dse_graph_element_ce);
  dse_graph_vertex_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
