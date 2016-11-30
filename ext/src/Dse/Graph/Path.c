#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_path_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_path_methods[] = {
  PHP_ABSTRACT_ME(DseGraphPath, labels,   arginfo_none)
  PHP_ABSTRACT_ME(DseGraphPath, objects,  arginfo_none)
  PHP_ABSTRACT_ME(DseGraphPath, hasLabel, arginfo_none)
  PHP_ABSTRACT_ME(DseGraphPath, object,   arginfo_name)
  PHP_FE_END
};

void dse_define_GraphPath(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Path", dse_graph_path_methods);
  dse_graph_path_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_path_ce TSRMLS_CC, 1, dse_graph_element_ce);
  dse_graph_path_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
