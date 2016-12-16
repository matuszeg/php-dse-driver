#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_element_ce = NULL;

void
php_driver_graph_element_init(php_driver_graph_element_base * element)
{
  PHP5TO7_ZVAL_UNDEF(element->id);
  element->label = NULL;

  PHP5TO7_ZVAL_MAYBE_MAKE(element->properties);
  array_init(PHP5TO7_ZVAL_MAYBE_P(element->properties));
}

void
php_driver_graph_element_destroy(php_driver_graph_element_base * element)
{
  PHP5TO7_ZVAL_MAYBE_DESTROY(element->id);
  if (element->label) {
    efree(element->label);
  }
  PHP5TO7_ZVAL_MAYBE_DESTROY(element->properties);
}

int
php_driver_graph_element_header_populate(HashTable *ht,
                                         php_driver_graph_element_base* element TSRMLS_DC)
{
  php5to7_zval *value;
  php_driver_graph_result *result;

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "id", sizeof("id"), value)) {
    return FAILURE;
  }
  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(element->id),
                    PHP5TO7_ZVAL_MAYBE_DEREF(value));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "label", sizeof("label"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING) {
    return FAILURE;
  }
  element->label = estrdup(PHP5TO7_Z_STRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

void
php_driver_graph_element_properties(php_driver_graph_element_base* element,
                                    HashTable *props TSRMLS_DC)
{
  php5to7_zval value;

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(element->id), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "id", sizeof("id"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(value),
                      element->label);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "label", sizeof("label"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(element->properties), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "properties", sizeof("properties"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_element_methods[] = {
  PHP_ABSTRACT_ME(GraphElement, id,         arginfo_none)
  PHP_ABSTRACT_ME(GraphElement, label,      arginfo_none)
  PHP_ABSTRACT_ME(GraphElement, properties, arginfo_none)
  PHP_ABSTRACT_ME(GraphElement, property,   arginfo_name)
  PHP_FE_END
};

void php_driver_define_GraphElement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Element", php_driver_graph_element_methods);
  php_driver_graph_element_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_graph_element_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
