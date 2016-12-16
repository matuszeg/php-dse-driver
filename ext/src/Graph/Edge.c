#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_edge_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_edge_methods[] = {
  PHP_ABSTRACT_ME(GraphEdge, inV,       arginfo_none)
  PHP_ABSTRACT_ME(GraphEdge, inVLabel,  arginfo_none)
  PHP_ABSTRACT_ME(GraphEdge, outV,      arginfo_none)
  PHP_ABSTRACT_ME(GraphEdge, outVLabel, arginfo_none)
  PHP_FE_END
};

void php_driver_define_GraphEdge(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Edge", php_driver_graph_edge_methods);
  php_driver_graph_edge_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_edge_ce TSRMLS_CC, 1, php_driver_graph_element_ce);
  php_driver_graph_edge_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
