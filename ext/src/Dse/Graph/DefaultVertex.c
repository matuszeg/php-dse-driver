#include "php_dse.h"
#include "php_dse_types.h"

#include "DefaultElement.h"
#include "Element.h"

zend_class_entry *dse_graph_default_vertex_ce = NULL;

int
php_dse_graph_default_vertex_construct(HashTable *ht,
                                       zval *return_value TSRMLS_DC)
{
  char *name;
  php5to7_zval *current;
  php5to7_zval *value;
  dse_graph_vertex *vertex;
  dse_graph_result *result;

  object_init_ex(return_value, dse_graph_default_vertex_ce);
  vertex = PHP_DSE_GET_GRAPH_VERTEX(return_value);

  if (php_dse_graph_element_header_populate(ht, &vertex->element TSRMLS_CC) == FAILURE) {
    return FAILURE;
  }

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "properties", sizeof("properties"), value)) {
    return FAILURE;
  }
  result = PHP_DSE_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_ARRAY) {
    return FAILURE;
  }

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value), name, current) {
    dse_graph_vertex_property *vertex_property;
    php5to7_zval zvertex_property;

    PHP5TO7_ZVAL_MAYBE_MAKE(zvertex_property);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zvertex_property), dse_graph_default_vertex_property_ce);
    vertex_property = PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(PHP5TO7_ZVAL_MAYBE_P(zvertex_property));

    if (php_dse_graph_default_element_populate(ht,
                                               return_value,
                                               &vertex_property->element TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }

    vertex_property->property.name = estrdup(name);
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(vertex_property->property.value),
                     PHP5TO7_ZVAL_MAYBE_DEREF(current));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(vertex_property->property.parent),
                      return_value);

    PHP5TO7_ZEND_HASH_ADD(&vertex->element.properties,
                          name, strlen(name) + 1,
                          PHP5TO7_ZVAL_MAYBE_P(zvertex_property), sizeof(zval *));
  } PHP5TO7_ZEND_HASH_FOREACH_END(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

PHP_METHOD(DseGraphDefaultVertex, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\DefaultVertex objects directly is not supported.");
  return;
}

PHP_METHOD(DseGraphDefaultVertex, id)
{
  dse_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->element.id), 0, 0)
}

PHP_METHOD(DseGraphDefaultVertex, label)
{
  dse_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX(getThis());

  PHP5TO7_RETURN_STRING(self->element.label);
}

PHP_METHOD(DseGraphDefaultVertex, properties)
{
  dse_graph_vertex *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_VERTEX(getThis());

  array_init(return_value);
  PHP5TO7_ZEND_HASH_ZVAL_COPY(Z_ARRVAL_P(return_value),
                              &self->element.properties);
}

PHP_METHOD(DseGraphDefaultVertex, property)
{
  char *name;
  php5to7_size name_len;
  dse_graph_vertex *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_VERTEX(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(&self->element.properties,
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

static zend_function_entry dse_graph_default_vertex_methods[] = {
  PHP_ME(DseGraphDefaultVertex, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  /* Graph element */
  PHP_ME(DseGraphDefaultVertex, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertex, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertex, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultVertex, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_default_vertex_handlers;

static HashTable *
php_dse_graph_default_vertex_properties(zval *object TSRMLS_DC)
{
  dse_graph_vertex  *self  = PHP_DSE_GET_GRAPH_VERTEX(object);
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  php_dse_graph_element_properties(&self->element, props TSRMLS_CC);

  return props;
}

static int
php_dse_graph_default_vertex_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_default_vertex_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_vertex *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_vertex, object);

  php_dse_graph_element_destroy(&self->element);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_default_vertex_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_vertex *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_vertex, ce);

  php_dse_graph_element_init(&self->element);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_vertex, dse_graph_default_vertex, self, ce);
}

void dse_define_GraphDefaultVertex(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\DefaultVertex", dse_graph_default_vertex_methods);
  dse_graph_default_vertex_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_default_vertex_ce TSRMLS_CC, 1, dse_graph_vertex_ce);
  dse_graph_default_vertex_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_default_vertex_ce->create_object = php_dse_graph_default_vertex_new;

  memcpy(&dse_graph_default_vertex_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_default_vertex_handlers.get_properties  = php_dse_graph_default_vertex_properties;
  dse_graph_default_vertex_handlers.compare_objects = php_dse_graph_default_vertex_compare;
  dse_graph_default_vertex_handlers.clone_obj = NULL;
}
