/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

#include "DefaultElement.h"
#include "Element.h"

zend_class_entry *php_driver_graph_default_vertex_ce = NULL;

int
php_driver_graph_default_vertex_construct(HashTable *ht,
                                          zval *return_value TSRMLS_DC)
{
  char *name;
  php5to7_zval *current;
  php5to7_zval *value;
  php_driver_graph_vertex *vertex;
  php_driver_graph_result *result;

  object_init_ex(return_value, php_driver_graph_default_vertex_ce);
  vertex = PHP_DRIVER_GET_GRAPH_VERTEX(return_value);

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "type", sizeof("type"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING ||
      strncmp(PHP5TO7_Z_STRVAL_MAYBE_P(result->value),
              "vertex",
              PHP5TO7_Z_STRLEN_MAYBE_P(result->value)) != 0) {
    return FAILURE;
  }

  if (php_driver_graph_element_header_populate(ht, &vertex->element TSRMLS_CC) == FAILURE) {
    return FAILURE;
  }

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "properties", sizeof("properties"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_ARRAY) {
    return FAILURE;
  }

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value), name, current) {
    php_driver_graph_vertex_property *vertex_property;
    php5to7_zval zvertex_property;

    PHP5TO7_ZVAL_MAYBE_MAKE(zvertex_property);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zvertex_property), php_driver_graph_default_vertex_property_ce);
    vertex_property = PHP_DRIVER_GET_GRAPH_VERTEX_PROPERTY(PHP5TO7_ZVAL_MAYBE_P(zvertex_property));

    if (php_driver_graph_default_element_populate(ht,
                                               return_value,
                                               &vertex_property->element TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }

    vertex_property->property.name = estrdup(name);
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(vertex_property->property.value),
                     PHP5TO7_ZVAL_MAYBE_DEREF(current));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(vertex_property->property.parent),
                      return_value);

    PHP5TO7_ZEND_HASH_ADD(PHP5TO7_Z_ARRVAL_MAYBE_P(vertex->element.properties),
                          name, strlen(name) + 1,
                          PHP5TO7_ZVAL_MAYBE_P(zvertex_property), sizeof(zval *));
  } PHP5TO7_ZEND_HASH_FOREACH_END(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

PHP_METHOD(GraphDefaultVertex, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\DefaultVertex objects directly is not supported.");
  return;
}

PHP_METHOD(GraphDefaultVertex, id)
{
  php_driver_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.id), 0, 0)
}

PHP_METHOD(GraphDefaultVertex, label)
{
  php_driver_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX(getThis());

  PHP5TO7_RETURN_STRING(self->element.label);
}

PHP_METHOD(GraphDefaultVertex, properties)
{
  php_driver_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_VERTEX(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.properties), 1, 0);
}

PHP_METHOD(GraphDefaultVertex, property)
{
  char *name;
  php5to7_size name_len;
  php_driver_graph_vertex *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_VERTEX(getThis());

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

static zend_function_entry php_driver_graph_default_vertex_methods[] = {
  PHP_ME(GraphDefaultVertex, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Graph element */
  PHP_ME(GraphDefaultVertex, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertex, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertex, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultVertex, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_default_vertex_handlers;

static HashTable *
php_driver_graph_default_vertex_properties(zval *object TSRMLS_DC)
{
  php_driver_graph_vertex  *self  = PHP_DRIVER_GET_GRAPH_VERTEX(object);
  HashTable                *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_graph_element_properties(&self->element, props TSRMLS_CC);

  return props;
}

static int
php_driver_graph_default_vertex_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_default_vertex_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_vertex *self = PHP5TO7_ZEND_OBJECT_GET(graph_vertex, object);

  php_driver_graph_element_destroy(&self->element);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_default_vertex_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_vertex *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_vertex, ce);

  php_driver_graph_element_init(&self->element);

  PHP5TO7_ZEND_OBJECT_INIT_EX(graph_vertex, graph_default_vertex, self, ce);
}

void php_driver_define_GraphDefaultVertex(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\DefaultVertex", php_driver_graph_default_vertex_methods);
  php_driver_graph_default_vertex_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_graph_vertex_ce);
  php_driver_graph_default_vertex_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_default_vertex_ce->create_object = php_driver_graph_default_vertex_new;

  memcpy(&php_driver_graph_default_vertex_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_default_vertex_handlers.get_properties  = php_driver_graph_default_vertex_properties;
  php_driver_graph_default_vertex_handlers.compare_objects = php_driver_graph_default_vertex_compare;
  php_driver_graph_default_vertex_handlers.clone_obj = NULL;
}
