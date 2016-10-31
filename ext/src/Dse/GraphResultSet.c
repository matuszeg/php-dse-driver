#include "php_dse.h"

zend_class_entry *dse_graph_result_set_ce = NULL;

static zend_function_entry dse_graph_result_set_methods[] = {
  PHP_FE_END
};

void dse_define_GraphResultSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\GraphResultSet", dse_graph_result_set_methods);
  dse_graph_result_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  //dse_graph_result_set_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
