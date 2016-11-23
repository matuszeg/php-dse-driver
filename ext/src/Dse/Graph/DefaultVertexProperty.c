#include "php_dse.h"
#include "php_dse_types.h"

#include "Element.h"
#include "Property.h"

zend_class_entry *dse_graph_default_vertex_property_ce = NULL;

PHP_METHOD(DseGraphDefaultVertexProperty, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\DefaultVertexProperty objects directly is not supported.");
  return;
}

PHP_METHOD(DseGraphDefaultVertexProperty, name)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->property.name);
}

PHP_METHOD(DseGraphDefaultVertexProperty, value)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->property.value), 0, 0)
}

PHP_METHOD(DseGraphDefaultVertexProperty, parent)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->property.parent), 0, 0)
}

PHP_METHOD(DseGraphDefaultVertexProperty, id)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.id), 0, 0)
}

PHP_METHOD(DseGraphDefaultVertexProperty, label)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->element.label);
}

PHP_METHOD(DseGraphDefaultVertexProperty, properties)
{
  dse_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.properties), 1, 0);
}

PHP_METHOD(DseGraphDefaultVertexProperty, property)
{
  char *name;
  php5to7_size name_len;
  dse_graph_vertex_property *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->element.properties),
                             name, name_len + 1,
                             result)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(result), 1, 0);
  }

  RETURN_FALSE;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_default_vertex_property_methods[] = {
  PHP_ME(DseGraphDefaultVertexProperty, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Graph property */
  PHP_ME(DseGraphDefaultVertexProperty, name,        arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertexProperty, value,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertexProperty, parent,      arginfo_none, ZEND_ACC_PUBLIC)
  /* Graph element */
  PHP_ME(DseGraphDefaultVertexProperty, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertexProperty, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertexProperty, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertexProperty, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_default_vertex_property_handlers;

static HashTable *
php_dse_graph_default_vertex_property_properties(zval *object TSRMLS_DC)
{
  dse_graph_vertex_property *self  = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(object);
  HashTable                 *props = zend_std_get_properties(object TSRMLS_CC);

  php_dse_graph_element_properties(&self->element, props TSRMLS_CC);
  php_dse_graph_property_properties(&self->property, props TSRMLS_CC);

  return props;
}

static int
php_dse_graph_default_vertex_property_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_default_vertex_property_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_vertex_property *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_vertex_property, object);

  php_dse_graph_element_destroy(&self->element);
  php_dse_graph_property_destroy(&self->property);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_default_vertex_property_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_vertex_property *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_vertex_property, ce);

  php_dse_graph_element_init(&self->element);
  php_dse_graph_property_init(&self->property);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_vertex_property, dse_graph_default_vertex_property, self, ce);
}

void dse_define_GraphDefaultVertexProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\DefaultVertexProperty", dse_graph_default_vertex_property_methods);
  dse_graph_default_vertex_property_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_default_vertex_property_ce TSRMLS_CC, 1, dse_graph_vertex_property_ce);
  dse_graph_default_vertex_property_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_default_vertex_property_ce->create_object = php_dse_graph_default_vertex_property_new;

  memcpy(&dse_graph_default_vertex_property_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_default_vertex_property_handlers.get_properties  = php_dse_graph_default_vertex_property_properties;
  dse_graph_default_vertex_property_handlers.compare_objects = php_dse_graph_default_vertex_property_compare;
  dse_graph_default_vertex_property_handlers.clone_obj = NULL;
}
