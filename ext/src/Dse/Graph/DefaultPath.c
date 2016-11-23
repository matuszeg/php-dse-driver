#include "php_dse.h"
#include "php_dse_types.h"

#include "DefaultPath.h"

zend_class_entry *dse_graph_default_path_ce = NULL;

static int
build_label(HashTable* label, php5to7_zval* return_value TSRMLS_DC)
{
  php5to7_zval *current;
  PHP5TO7_ZVAL_MAYBE_MAKE(*return_value);
  array_init(PHP5TO7_ZVAL_MAYBE_DEREF(return_value));

  PHP5TO7_ZEND_HASH_FOREACH_VAL(label, current) {
    dse_graph_result *result =
        PHP_DSE_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_STRING) {
      return FAILURE;
    }
    PHP5TO7_ADD_NEXT_INDEX_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(return_value),
                                  PHP5TO7_Z_STRVAL_MAYBE_P(result->value));
  } PHP5TO7_ZEND_HASH_FOREACH_END(label);

  return SUCCESS;
}

int
php_dse_graph_default_path_construct(HashTable *ht,
                                     zval *return_value TSRMLS_DC)
{
  php5to7_zval *current;
  php5to7_zval *value;
  dse_graph_result *result;
  dse_graph_path* path;

  object_init_ex(return_value, dse_graph_default_path_ce);
  path = PHP_DSE_GET_GRAPH_PATH(return_value);

  PHP5TO7_ZVAL_MAYBE_MAKE(path->labels);
  array_init(PHP5TO7_ZVAL_MAYBE_P(path->labels));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "labels", sizeof("labels"), value)) {
    return FAILURE;
  }
  result = PHP_DSE_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_ARRAY) {
    return FAILURE;
  }

  PHP5TO7_ZEND_HASH_FOREACH_VAL(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value), current) {
    php5to7_zval label;
    dse_graph_result *sub_result =
        PHP_DSE_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    if (PHP5TO7_Z_TYPE_MAYBE_P(sub_result->value) != IS_ARRAY) {
      return FAILURE;
    }

    PHP5TO7_ZVAL_UNDEF(label);

    if (build_label(PHP5TO7_Z_ARRVAL_MAYBE_P(sub_result->value), &label TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }

    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(PHP5TO7_Z_ARRVAL_MAYBE_P(path->labels),
                                        PHP5TO7_ZVAL_MAYBE_P(label),
                                        sizeof(zval *));
  } PHP5TO7_ZEND_HASH_FOREACH_END(PHP5TO7_Z_ARRVAL_MAYBE_P(result->value));

  if (!PHP5TO7_ZEND_HASH_FIND(ht, "objects", sizeof("objects"), value)) {
    return FAILURE;
  }
  result = PHP_DSE_GET_GRAPH_RESULT(PHP5TO7_ZVAL_MAYBE_DEREF(value));
  if (PHP5TO7_Z_TYPE_MAYBE_P(result->value) != IS_ARRAY) {
    return FAILURE;
  }

  PHP5TO7_ZVAL_MAYBE_MAKE(path->objects);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(path->objects),
            PHP5TO7_ZVAL_MAYBE_P(result->value), 1, 0);

  return SUCCESS;
}

PHP_METHOD(DseGraphDefaultPath, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Instantiation of a Dse\\Graph\\DefaultPath objects directly is not supported.");
  return;
}

PHP_METHOD(DseGraphDefaultPath, labels)
{
  dse_graph_path *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_PATH(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->labels), 1, 0);
}

PHP_METHOD(DseGraphDefaultPath, objects)
{
  dse_graph_path *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_GRAPH_PATH(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->objects), 1, 0);
}

PHP_METHOD(DseGraphDefaultPath, hasLabel)
{
  char *name;
  php5to7_size name_len;
  dse_graph_path *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_PATH(getThis());

  /* TODO: Implement */
  zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");
}

PHP_METHOD(DseGraphDefaultPath, object)
{
  char *name;
  php5to7_size name_len;
  dse_graph_path *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_PATH(getThis());

  /* TODO: Implement */
  zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");

  RETURN_FALSE;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_default_path_methods[] = {
  PHP_ME(DseGraphDefaultPath, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | PHP5TO7_ZEND_ACC_FINAL)
  PHP_ME(DseGraphDefaultPath, labels,      arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultPath, objects,     arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultPath, hasLabel,    arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphDefaultPath, object,      arginfo_name, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_default_path_handlers;

static HashTable *
php_dse_graph_default_path_properties(zval *object TSRMLS_DC)
{
  php5to7_zval value;

  dse_graph_path *self  = PHP_DSE_GET_GRAPH_PATH(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->labels), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "labels", sizeof("labels"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  ZVAL_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value),
            PHP5TO7_ZVAL_MAYBE_P(self->objects), 1, 0);
  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "objects", sizeof("objects"),
                           PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval *));

  return props;
}

static int
php_dse_graph_default_path_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_default_path_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_path *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_path, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->labels);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->objects);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_default_path_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_path *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_path, ce);

  PHP5TO7_ZVAL_UNDEF(self->labels);
  PHP5TO7_ZVAL_UNDEF(self->objects);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_path, dse_graph_default_path, self, ce);
}

void dse_define_GraphDefaultPath(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\DefaultPath", dse_graph_default_path_methods);
  dse_graph_default_path_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_default_path_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_default_path_ce->create_object = php_dse_graph_default_path_new;

  memcpy(&dse_graph_default_path_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_default_path_handlers.get_properties  = php_dse_graph_default_path_properties;
  dse_graph_default_path_handlers.compare_objects = php_dse_graph_default_path_compare;
  dse_graph_default_path_handlers.clone_obj = NULL;
}
