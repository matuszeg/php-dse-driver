#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_property_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_property_methods[] = {
  PHP_ABSTRACT_ME(DseGraphProperty, name,   arginfo_none)
  PHP_ABSTRACT_ME(DseGraphProperty, value,  arginfo_none)
  PHP_ABSTRACT_ME(DseGraphProperty, parent, arginfo_none)
  PHP_FE_END
};

void dse_define_GraphProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Property", dse_graph_property_methods);
  dse_graph_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_property_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
