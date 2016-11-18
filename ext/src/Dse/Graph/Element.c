#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_element_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_element_methods[] = {
  PHP_ABSTRACT_ME(DseGraphElement, id,         arginfo_none)
  PHP_ABSTRACT_ME(DseGraphElement, label,      arginfo_none)
  PHP_ABSTRACT_ME(DseGraphElement, properties, arginfo_none)
  PHP_ABSTRACT_ME(DseGraphElement, property,   arginfo_name)
  PHP_FE_END
};

void dse_define_GraphElement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Element", dse_graph_element_methods);
  dse_graph_element_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_element_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
