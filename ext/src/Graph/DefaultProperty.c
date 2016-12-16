#include "php_driver.h"
#include "php_driver_types.h"

#include "Property.h"

zend_class_entry *php_driver_graph_default_property_ce = NULL;

PHP_METHOD(GraphDefaultProperty, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\DefaultProperty objects directly is not supported.");
  return;
}

PHP_METHOD(GraphDefaultProperty, name)
{
  php_driver_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->base.name);
}

PHP_METHOD(GraphDefaultProperty, value)
{
  php_driver_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.value), 0, 0)
}

PHP_METHOD(GraphDefaultProperty, parent)
{
  php_driver_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.parent), 0, 0)
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_default_property_methods[] = {
  PHP_ME(GraphDefaultProperty, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(GraphDefaultProperty, name,        arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultProperty, value,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultProperty, parent,      arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_default_property_handlers;

static HashTable *
php_driver_graph_default_property_properties(zval *object TSRMLS_DC)
{
  php_driver_graph_property *self  = PHP_DRIVER_GET_GRAPH_PROPERTY(object);
  HashTable                 *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_graph_property_properties(&self->base, props TSRMLS_CC);

  return props;
}

static int
php_driver_graph_default_property_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_default_property_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_property *self = PHP5TO7_ZEND_OBJECT_GET(graph_property, object);

  php_driver_graph_property_destroy(&self->base);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_default_property_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_property *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_property, ce);

  php_driver_graph_property_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT_EX(graph_property, graph_default_property, self, ce);
}

void php_driver_define_GraphDefaultProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\DefaultProperty", php_driver_graph_default_property_methods);
  php_driver_graph_default_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_default_property_ce TSRMLS_CC, 1, php_driver_graph_property_ce);
  php_driver_graph_default_property_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_default_property_ce->create_object = php_driver_graph_default_property_new;

  memcpy(&php_driver_graph_default_property_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_default_property_handlers.get_properties  = php_driver_graph_default_property_properties;
  php_driver_graph_default_property_handlers.compare_objects = php_driver_graph_default_property_compare;
  php_driver_graph_default_property_handlers.clone_obj = NULL;
}
