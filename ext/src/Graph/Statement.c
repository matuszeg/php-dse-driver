#include "php_driver.h"

zend_class_entry *php_driver_graph_statement_ce = NULL;

static zend_function_entry php_driver_graph_statement_methods[] = {
  PHP_FE_END
};

void php_driver_define_GraphStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Statement", php_driver_graph_statement_methods);
  php_driver_graph_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_graph_statement_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
