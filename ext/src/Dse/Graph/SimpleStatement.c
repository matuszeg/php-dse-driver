#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_simple_statement_ce = NULL;

PHP_METHOD(DseGraphSimpleStatement, __construct)
{
  zval *query = NULL;
  dse_graph_statement *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &query) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(query) != IS_STRING) {
    INVALID_ARGUMENT(query, "a string");
  }

  self = PHP_DSE_GET_GRAPH_STATEMENT(getThis());

  self->query = estrndup(Z_STRVAL_P(query), Z_STRLEN_P(query));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_simple_statement_methods[] = {
  PHP_ME(DseGraphSimpleStatement, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers dse_graph_simple_statement_handlers;

static HashTable *
php_dse_graph_simple_statement_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_graph_simple_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_simple_statement_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_statement *self =
      PHP5TO7_ZEND_OBJECT_GET(dse_graph_statement, object);

  if (self->query) {
    efree(self->query);
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_simple_statement_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_statement *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_statement, ce);

  self->query = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_graph_statement, dse_graph_simple_statement, self, ce);
}

void dse_define_GraphSimpleStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\SimpleStatement", dse_graph_simple_statement_methods);
  dse_graph_simple_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_graph_simple_statement_ce TSRMLS_CC, 1, dse_graph_statement_ce);
  dse_graph_simple_statement_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_simple_statement_ce->create_object = php_dse_graph_simple_statement_new;

  memcpy(&dse_graph_simple_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_simple_statement_handlers.get_properties  = php_dse_graph_simple_statement_properties;
  dse_graph_simple_statement_handlers.compare_objects = php_dse_graph_simple_statement_compare;
  dse_graph_simple_statement_handlers.clone_obj = NULL;
}
