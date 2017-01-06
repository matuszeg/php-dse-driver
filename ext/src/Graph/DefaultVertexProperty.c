/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

#include "Element.h"
#include "Property.h"

zend_class_entry *php_driver_graph_default_vertex_property_ce = NULL;

PHP_METHOD(GraphDefaultVertexProperty, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\DefaultVertexProperty objects directly is not supported.");
  return;
}

PHP_METHOD(GraphDefaultVertexProperty, name)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->property.name);
}

PHP_METHOD(GraphDefaultVertexProperty, value)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->property.value), 0, 0)
}

PHP_METHOD(GraphDefaultVertexProperty, parent)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->property.parent), 0, 0)
}

PHP_METHOD(GraphDefaultVertexProperty, id)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.id), 0, 0)
}

PHP_METHOD(GraphDefaultVertexProperty, label)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  PHP5TO7_RETURN_STRING(self->element.label);
}

PHP_METHOD(GraphDefaultVertexProperty, properties)
{
  php_driver_graph_vertex_property *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.properties), 1, 0);
}

PHP_METHOD(GraphDefaultVertexProperty, property)
{
  char *name;
  php5to7_size name_len;
  php_driver_graph_vertex_property *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(getThis());

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

static zend_function_entry php_driver_graph_default_vertex_property_methods[] = {
  PHP_ME(GraphDefaultVertexProperty, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Graph property */
  PHP_ME(GraphDefaultVertexProperty, name,        arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertexProperty, value,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertexProperty, parent,      arginfo_none, ZEND_ACC_PUBLIC)
  /* Graph element */
  PHP_ME(GraphDefaultVertexProperty, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertexProperty, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertexProperty, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertexProperty, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_default_vertex_property_handlers;

static HashTable *
php_driver_graph_default_vertex_property_properties(zval *object TSRMLS_DC)
{
  php_driver_graph_vertex_property *self  = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(object);
  HashTable                        *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_graph_element_properties(&self->element, props TSRMLS_CC);
  php_driver_graph_property_properties(&self->property, props TSRMLS_CC);

  return props;
}

static int
php_driver_graph_default_vertex_property_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_default_vertex_property_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_vertex_property *self = PHP5TO7_ZEND_OBJECT_GET(graph_vertex_property, object);

  php_driver_graph_element_destroy(&self->element);
  php_driver_graph_property_destroy(&self->property);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_default_vertex_property_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_vertex_property *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_vertex_property, ce);

  php_driver_graph_element_init(&self->element);
  php_driver_graph_property_init(&self->property);

  PHP5TO7_ZEND_OBJECT_INIT_EX(graph_vertex_property, graph_default_vertex_property, self, ce);
}

void php_driver_define_GraphDefaultVertexProperty(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\DefaultVertexProperty", php_driver_graph_default_vertex_property_methods);
  php_driver_graph_default_vertex_property_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_graph_vertex_property_ce);
  php_driver_graph_default_vertex_property_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_default_vertex_property_ce->create_object = php_driver_graph_default_vertex_property_new;

  memcpy(&php_driver_graph_default_vertex_property_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_default_vertex_property_handlers.get_properties  = php_driver_graph_default_vertex_property_properties;
  php_driver_graph_default_vertex_property_handlers.compare_objects = php_driver_graph_default_vertex_property_compare;
  php_driver_graph_default_vertex_property_handlers.clone_obj = NULL;
}
