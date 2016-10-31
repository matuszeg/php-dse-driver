#include "php_dse.h"

zend_class_entry *dse_graph_statement_ce = NULL;

static zend_function_entry dse_graph_statement_methods[] = {
  PHP_FE_END
};

void dse_define_GraphStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\GraphStatement", dse_graph_statement_methods);
  dse_graph_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_statement_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
