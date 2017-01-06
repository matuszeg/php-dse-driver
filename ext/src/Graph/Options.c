/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry *php_driver_graph_options_ce = NULL;

static zend_function_entry php_driver_graph_options_methods[] = {
  PHP_FE_END
};

static zend_object_handlers php_driver_graph_options_handlers;

static HashTable *
php_driver_graph_options_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_graph_options_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_graph_options_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_graph_options *self = PHP5TO7_ZEND_OBJECT_GET(graph_options, object);

  dse_graph_options_free(self->options);
  if (self->hash_key) {
    efree(self->hash_key);
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_graph_options_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_graph_options *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(graph_options, ce);

  self->options = dse_graph_options_new();
  self->hash_key = NULL;

  PHP5TO7_ZEND_OBJECT_INIT(graph_options, self, ce);
}

void php_driver_define_GraphOptions(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Graph\\Options", php_driver_graph_options_methods);
  php_driver_graph_options_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_graph_options_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_graph_options_ce->create_object = php_driver_graph_options_new;

  memcpy(&php_driver_graph_options_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_graph_options_handlers.get_properties  = php_driver_graph_options_properties;
  php_driver_graph_options_handlers.compare_objects = php_driver_graph_options_compare;
  php_driver_graph_options_handlers.clone_obj = NULL;
}
