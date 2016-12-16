#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_path_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_path_methods[] = {
  PHP_ABSTRACT_ME(GraphPath, labels,   arginfo_none)
  PHP_ABSTRACT_ME(GraphPath, objects,  arginfo_none)
  PHP_ABSTRACT_ME(GraphPath, hasLabel, arginfo_none)
  PHP_ABSTRACT_ME(GraphPath, object,   arginfo_name)
  PHP_FE_END
};

void php_driver_define_GraphPath(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Path", php_driver_graph_path_methods);
  php_driver_graph_path_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_path_ce TSRMLS_CC, 1, php_driver_graph_element_ce);
  php_driver_graph_path_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
