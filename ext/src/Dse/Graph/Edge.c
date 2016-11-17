#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_graph_edge_ce = NULL;

PHP_METHOD(DseGraphEdge, __construct)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_edge_methods[] = {
  PHP_ME(DseGraphEdge, __construct,  arginfo_none,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers dse_graph_edge_handlers;

static HashTable *
php_dse_graph_edge_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_graph_edge_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_edge_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_edge *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_edge, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_edge_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_edge *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_edge, ce);

  /* Initialize */

  PHP5TO7_ZEND_OBJECT_INIT(dse_graph_edge, self, ce);
}

void dse_define_GraphEdge(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Edge", dse_graph_edge_methods);
  dse_graph_edge_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_edge_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_edge_ce->create_object = php_dse_graph_edge_new;

  memcpy(&dse_graph_edge_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_edge_handlers.get_properties  = php_dse_graph_edge_properties;
  dse_graph_edge_handlers.compare_objects = php_dse_graph_edge_compare;
  dse_graph_edge_handlers.clone_obj = NULL;
}
