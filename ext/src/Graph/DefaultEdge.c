/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

#include "DefaultElement.h"
#include "Element.h"

zend_class_entry *php_driver_graph_default_edge_ce = NULL;

int
php_driver_graph_default_edge_construct(HashTable *ht,
                                        zval *return_value TSRMLS_DC)
{
  php5to7_zval *value;
  php_driver_graph_result *result;
  php_driver_graph_edge* edge;

  object_init_ex(return_value, php_driver_graph_default_edge_ce);
  edge = PHP_DRIVER_GET_GRAPH_EDGE(return_value);

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "type", sizeof("type"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING ||
      strncmp(PHP5TO7_Z_STRVAL_MAYBE_P(result->value),
              "edge",
              PHP5TO7_Z_STRLEN_MAYBE_P(result->value)) != 0) {
    return FAILURE;
  }

  if (php_driver_graph_default_element_populate(ht,
                                             return_value,
                                             &edge->element TSRMLS_CC) == FAILURE) {
    return FAILURE;
  }

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "inV", sizeof("inV"), value)) {
    return FAILURE;
  }
  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(edge->in_v),
                    PHP5TO7_ZVAL_MAYBE_DEREF(value));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "inVLabel", sizeof("inVLabel"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING) {
    return FAILURE;
  }
  edge->in_v_label = estrdup(PHP5TO7_Z_STRVAL_MAYBE_P(result->value));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "outV", sizeof("outV"), value)) {
    return FAILURE;
  }
  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(edge->out_v),
                    PHP5TO7_ZVAL_MAYBE_DEREF(value));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "outVLabel", sizeof("outVLabel"), value)) {
    return FAILURE;
  }
  result = PHP_DRIVER_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING) {
    return FAILURE;
  }
  edge->out_v_label = estrdup(PHP5TO7_Z_STRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

PHP_METHOD(GraphDefaultEdge, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\DefaultEdge objects directly is not supported.");
  return;
}

PHP_METHOD(GraphDefaultEdge, id)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.id), 0, 0)
}

PHP_METHOD(GraphDefaultEdge, label)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  PHP5TO7_RETURN_STRING(self->element.label);
}

PHP_METHOD(GraphDefaultEdge, properties)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.properties), 1, 0);
}

PHP_METHOD(GraphDefaultEdge, property)
{
  char *name;
  php5to7_size name_len;
  php_driver_graph_edge *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->element.properties),
                             name, name_len + 1,
                             result)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(result), 1, 0);
  }

  RETURN_FALSE;
}

PHP_METHOD(GraphDefaultEdge, inV)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->in_v), 0, 0)
}

PHP_METHOD(GraphDefaultEdge, inVLabel)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  PHP5TO7_RETURN_STRING(self->in_v_label);
}

PHP_METHOD(GraphDefaultEdge, outV)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->out_v), 0, 0)
}

PHP_METHOD(GraphDefaultEdge, outVLabel)
{
  php_driver_graph_edge *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_EDGE(getThis());

  PHP5TO7_RETURN_STRING(self->out_v_label);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_graph_default_edge_methods[] = {
  PHP_ME(GraphDefaultEdge, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Graph element */
  PHP_ME(GraphDefaultEdge, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, property,    arginfo_name, ZEND_ACC_PUBLIC)
  /* Graph edge */
  PHP_ME(GraphDefaultEdge, inV,         arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, inVLabel,    arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, outV,        arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultEdge, outVLabel,   arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_default_edge_handlers;

static HashTable *
php_driver_graph_default_edge_properties(zval *object TSRMLS_DC)
{
  php5to7_zval value;

  php_driver_graph_edge *self  = PHP_DRIVER_GET_GRAPH_EDGE(object);
  HashTable             *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_graph_element_properties(&self->element, props TSRMLS_CC);

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->in_v), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "inV", sizeof("inV"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(value), self->in_v_label);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "inVLabel", sizeof("inVLabel"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->out_v), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "outV", sizeof("outV"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(value), self->out_v_label);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "outVLabel", sizeof("outVLabel"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  return props;
}

static int
php_driver_graph_default_edge_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_default_edge_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_edge *self = PHP5TO7_ZEND_OBJECT_GET(graph_edge, object);

  php_driver_graph_element_destroy(&self->element);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->in_v);
  if (self->in_v_label) {
    efree(self->in_v_label);
  }
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->out_v);
  if (self->out_v_label) {
    efree(self->out_v_label);
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_default_edge_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_edge *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_edge, ce);

  php_driver_graph_element_init(&self->element);
  PHP5TO7_ZVAL_UNDEF(self->in_v);
  self->in_v_label = NULL;
  PHP5TO7_ZVAL_UNDEF(self->out_v);
  self->out_v_label = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(graph_edge, graph_default_edge, self, ce);
}

void php_driver_define_GraphDefaultEdge(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\DefaultEdge", php_driver_graph_default_edge_methods);
  php_driver_graph_default_edge_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_graph_edge_ce);
  php_driver_graph_default_edge_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_default_edge_ce->create_object = php_driver_graph_default_edge_new;

  memcpy(&php_driver_graph_default_edge_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_default_edge_handlers.get_properties  = php_driver_graph_default_edge_properties;
  php_driver_graph_default_edge_handlers.compare_objects = php_driver_graph_default_edge_compare;
  php_driver_graph_default_edge_handlers.clone_obj = NULL;
}
