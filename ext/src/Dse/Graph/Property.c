#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_property_ce = NULL;

void
php_dse_graph_property_init(dse_graph_property_base * property)
{
  property->name = NULL;
  PHP5TO7_ZVAL_UNDEF(property->value);
  PHP5TO7_ZVAL_UNDEF(property->parent);
}

void
php_dse_graph_property_destroy(dse_graph_property_base * property)
{
  if (property->name) {
    efree(property->name);
  }
  PHP5TO7_ZVAL_MAYBE_DESTROY(property->value);
  PHP5TO7_ZVAL_MAYBE_DESTROY(property->parent);
}

void
php_dse_graph_property_properties(dse_graph_property_base* property,
                                  HashTable *props TSRMLS_DC)
{
  php5to7_zval value;

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(value),
                      property->name);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "name", sizeof("name"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(property->value), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "value", sizeof("value"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));
}

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
