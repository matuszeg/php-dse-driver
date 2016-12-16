#include "php_driver.h"
#include "php_driver_types.h"

#include "DefaultElement.h"
#include "Element.h"

zend_class_entry *php_driver_graph_default_element_ce = NULL;

int
php_driver_graph_default_element_populate(HashTable *ht,
                                          zval *element,
                                          php_driver_graph_element_base* element_base TSRMLS_DC)
{
  char *name;
  php5to7_zval *current;
  php5to7_zval *value;
  php_driver_graph_result *result;

  if (php_driver_graph_element_header_populate(ht, element_base TSRMLS_CC) == FAILURE) {
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
    php_driver_graph_property *property;
    php5to7_zval zproperty;

    PHP5TO7_ZVAL_MAYBE_MAKE(zproperty);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zproperty), php_driver_graph_default_property_ce);
    property = PHP_DRIVER_GET_GRAPH_PROPERTY(PHP5TO7_ZVAL_MAYBE_P(zproperty));

    property->base.name = estrdup(name);
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(property->base.value),
                      PHP5TO7_ZVAL_MAYBE_DEREF(current));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(property->base.parent), element);

    PHP5TO7_ZEND_HASH_ADD(PHP5TO7_Z_ARRVAL_MAYBE_P(element_base->properties),
                          name, strlen(name) + 1,
                          PHP5TO7_ZVAL_MAYBE_P(zproperty), sizeof(zval *));
  } PHP5TO7_ZEND_HASH_FOREACH_END(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value));

  return SUCCESS;
}

PHP_METHOD(GraphDefaultElement, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Graph\\DefaultEdge objects directly is not supported.");
  return;
}

PHP_METHOD(GraphDefaultElement, id)
{
  php_driver_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_ELEMENT(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.id), 0, 0)
}

PHP_METHOD(GraphDefaultElement, label)
{
  php_driver_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_ELEMENT(getThis());

  PHP5TO7_RETURN_STRING(self->base.label);
}

PHP_METHOD(GraphDefaultElement, properties)
{
  php_driver_graph_element *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_GRAPH_ELEMENT(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base.properties), 1, 0);
}

PHP_METHOD(GraphDefaultElement, property)
{
  char *name;
  php5to7_size name_len;
  php_driver_graph_element *self = NULL;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_GRAPH_ELEMENT(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->base.properties),
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

static zend_function_entry php_driver_graph_default_element_methods[] = {
  PHP_ME(GraphDefaultElement, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(GraphDefaultElement, id,          arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultElement, label,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultElement, properties,  arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(GraphDefaultElement, property,    arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_default_element_handlers;

static HashTable *
php_driver_graph_default_element_properties(zval *object TSRMLS_DC)
{
  php_driver_graph_element *self  = PHP_DRIVER_GET_GRAPH_ELEMENT(object);
  HashTable                *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_graph_element_properties(&self->base, props TSRMLS_CC);

  return props;
}

static int
php_driver_graph_default_element_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_default_element_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_element *self = PHP5TO7_ZEND_OBJECT_GET(graph_element, object);

  php_driver_graph_element_destroy(&self->base);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_default_element_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_element *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_element, ce);

  php_driver_graph_element_init(&self->base);

  PHP5TO7_ZEND_OBJECT_INIT_EX(graph_element, graph_default_element, self, ce);
}

void php_driver_define_GraphDefaultElement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\DefaultElement", php_driver_graph_default_element_methods);
  php_driver_graph_default_element_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_graph_default_element_ce TSRMLS_CC, 1, php_driver_graph_element_ce);
  php_driver_graph_default_element_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_default_element_ce->create_object = php_driver_graph_default_element_new;

  memcpy(&php_driver_graph_default_element_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_default_element_handlers.get_properties  = php_driver_graph_default_element_properties;
  php_driver_graph_default_element_handlers.compare_objects = php_driver_graph_default_element_compare;
  php_driver_graph_default_element_handlers.clone_obj = NULL;
}
