#include "php_dse.h"
#include "php_dse_types.h"

#include "Property.h"

zend_class_entry *dse_graph_default_property_ce = NULL;

PHP_METHOD(DseGraphDefaultProperty, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\DefaultProperty objects directly is not supported.");
  return;
}

PHP_METHOD(DseGraphDefaultProperty, name)
{
  dse_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->base.name);
}

PHP_METHOD(DseGraphDefaultProperty, value)
{
  dse_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.value), 0, 0)
}

PHP_METHOD(DseGraphDefaultProperty, parent)
{
  dse_graph_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.parent), 0, 0)
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_default_property_methods[] = {
  PHP_ME(DseGraphDefaultProperty, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(DseGraphDefaultProperty, name,        arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultProperty, value,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultProperty, parent,      arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_default_property_handlers;

static HashTable *
php_dse_graph_default_property_properties(zval *object TSRMLS_DC)
{
  dse_graph_property *self  = PHP_DSE_GET_GRAPH_PROPERTY(object);
  HashTable          *props = zend_std_get_properties(object TSRMLS_CC);

  php_dse_graph_property_properties(&self->base, props TSRMLS_CC);

  return props;
}

static int
php_dse_graph_default_property_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_default_property_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_property *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_property, object);

  php_dse_graph_property_destroy(&self->base);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_default_property_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_property *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_property, ce);

  php_dse_graph_property_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_property, dse_graph_default_property, self, ce);
}

void dse_define_GraphDefaultProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\DefaultProperty", dse_graph_default_property_methods);
  dse_graph_default_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_default_property_ce TSRMLS_CC, 1, dse_graph_property_ce);
  dse_graph_default_property_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_default_property_ce->create_object = php_dse_graph_default_property_new;

  memcpy(&dse_graph_default_property_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_default_property_handlers.get_properties  = php_dse_graph_default_property_properties;
  dse_graph_default_property_handlers.compare_objects = php_dse_graph_default_property_compare;
  dse_graph_default_property_handlers.clone_obj = NULL;
}
