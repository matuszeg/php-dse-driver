#include "php_dse.h"

zend_class_entry *dse_graph_result_ce = NULL;

static zend_function_entry dse_graph_result_methods[] = {
  PHP_FE_END
};

void dse_define_GraphResult(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\GraphResult", dse_graph_result_methods);
  dse_graph_result_ce = zend_register_internal_class(&ce TSRMLS_CC);
  //dse_graph_result_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
