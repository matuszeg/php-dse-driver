#include "php_dse.h"
#include "php_dse_types.h"

#include "DefaultElement.h"
#include "Element.h"

zend_class_entry *dse_graph_default_element_ce = NULL;

int
php_dse_graph_default_element_populate(HashTable *ht,
                                       zval *element,
                                       dse_graph_element_base* element_base TSRMLS_DC)
{
  char *name;
  php5to7_zval *current;
  php5to7_zval *value;
  dse_graph_result *result;

  if (php_dse_graph_element_header_populate(ht, element_base TSRMLS_CC) == FAILURE) {
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
    dse_graph_property *property;
    php5to7_zval zproperty;

    PHP5TO7_ZVAL_MAYBE_MAKE(zproperty);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zproperty), dse_graph_default_property_ce);
    property = PHP_DSE_GET_GRAPH_PROPERTY(PHP5TO7_ZVAL_MAYBE_P(zproperty));

    property->base.name = estrdup(name);
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(property->base.value),
                      PHP5TO7_ZVAL_MAYBE_DEREF(current));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(property->base.parent), element);

    PHP5TO7_ZEND_HASH_ADD(&element_base->properties,
                          name, strlen(name) + 1,
                          PHP5TO7_ZVAL_MAYBE_P(zproperty), sizeof(zval *));
  } PHP5TO7_ZEND_HASH_FOREACH_END(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

PHP_METHOD(DseGraphDefaultElement, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\DefaultEdge objects directly is not supported.");
  return;
}

PHP_METHOD(DseGraphDefaultElement, id)
{
  dse_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_ELEMENT(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.id), 0, 0)
}

PHP_METHOD(DseGraphDefaultElement, label)
{
  dse_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_ELEMENT(getThis());

  PHP5TO7_RETURN_STRING(self->base.label);
}

PHP_METHOD(DseGraphDefaultElement, properties)
{
  dse_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_ELEMENT(getThis());

  array_init(return_value);
  PHP5TO7_ZEND_HASH_ZVAL_COPY(Z_ARRVAL_P(return_value),
                              &self->base.properties);
}

PHP_METHOD(DseGraphDefaultElement, property)
{
  char *name;
  php5to7_size name_len;
  dse_graph_element *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_ELEMENT(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(&self->base.properties,
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

static zend_function_entry dse_graph_default_element_methods[] = {
  PHP_ME(DseGraphDefaultElement, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(DseGraphDefaultElement, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultElement, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultElement, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultElement, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_default_element_handlers;

static HashTable *
php_dse_graph_default_element_properties(zval *object TSRMLS_DC)
{
  dse_graph_element *self  = PHP_DSE_GET_GRAPH_ELEMENT(object);
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  php_dse_graph_element_properties(&self->base, props TSRMLS_CC);

  return props;
}

static int
php_dse_graph_default_element_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_default_element_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_element *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_element, object);

  php_dse_graph_element_destroy(&self->base);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_default_element_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_element *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_element, ce);

  php_dse_graph_element_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_element, dse_graph_default_element, self, ce);
}

void dse_define_GraphDefaultElement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\DefaultElement", dse_graph_default_element_methods);
  dse_graph_default_element_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_default_element_ce TSRMLS_CC, 1, dse_graph_element_ce);
  dse_graph_default_element_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_default_element_ce->create_object = php_dse_graph_default_element_new;

  memcpy(&dse_graph_default_element_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_default_element_handlers.get_properties  = php_dse_graph_default_element_properties;
  dse_graph_default_element_handlers.compare_objects = php_dse_graph_default_element_compare;
  dse_graph_default_element_handlers.clone_obj = NULL;
}
