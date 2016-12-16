#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_edge_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_edge_methods[] = {
  PHP_ABSTRACT_ME(DseGraphEdge, inV,       arginfo_none)
  PHP_ABSTRACT_ME(DseGraphEdge, inVLabel,  arginfo_none)
  PHP_ABSTRACT_ME(DseGraphEdge, outV,      arginfo_none)
  PHP_ABSTRACT_ME(DseGraphEdge, outVLabel, arginfo_none)
  PHP_FE_END
};

void dse_define_GraphEdge(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Edge", dse_graph_edge_methods);
  dse_graph_edge_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_edge_ce TSRMLS_CC, 1, dse_graph_element_ce);
  dse_graph_edge_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
