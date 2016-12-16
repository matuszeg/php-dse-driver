#include "php_dse.h"
#include "php_dse_types.h"

#include "util/consistency.h"

zend_class_entry *dse_graph_options_builder_ce = NULL;

PHP_METHOD(DseGraphOptionsBuilder, build)
{
  dse_graph_options *options;

  dse_graph_options_builder *self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());

  object_init_ex(return_value, dse_graph_options_ce);
  options = PHP_DSE_GET_GRAPH_OPTIONS(return_value);

  if (self->graph_language) {
    dse_graph_options_set_graph_language(options->options, self->graph_language);
  }

  if (self->graph_source) {
    dse_graph_options_set_graph_source(options->options, self->graph_source);
  }

  if (self->graph_name) {
    dse_graph_options_set_graph_name(options->options, self->graph_name);
  }

  if (self->read_consistency != CASS_CONSISTENCY_UNKNOWN) {
    dse_graph_options_set_read_consistency(options->options, self->read_consistency);
  }

  if (self->write_consistency != CASS_CONSISTENCY_UNKNOWN) {
    dse_graph_options_set_write_consistency(options->options, self->write_consistency);
  }

  if (self->request_timeout > 0) {
    dse_graph_options_set_request_timeout(options->options, self->request_timeout);
  }

  spprintf(&options->hash_key, 0,
           ":graph_options:%s:%s:%s:%d:%d:%d",
           SAFE_STR(self->graph_language),
           SAFE_STR(self->graph_source),
           SAFE_STR(self->graph_name),
           (int) self->read_consistency,
           (int) self->write_consistency,
           (int) self->request_timeout);
}

PHP_METHOD(DseGraphOptionsBuilder, withGraphLanguage)
{
  char *language;
  php5to7_size language_len;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &language, &language_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  self->graph_language = estrndup(language, language_len);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseGraphOptionsBuilder, withGraphSource)
{
  char *source;
  php5to7_size source_len;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &source, &source_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  self->graph_source = estrndup(source, source_len);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseGraphOptionsBuilder, withGraphName)
{
  char *name;
  php5to7_size name_len;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  self->graph_name = estrndup(name, name_len);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseGraphOptionsBuilder, withReadConsistency)
{
  zval *consistency = NULL;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",
                            &consistency) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  if (php_cassandra_get_consistency(consistency,
                                    &self->read_consistency TSRMLS_CC) == FAILURE) {
    return;
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseGraphOptionsBuilder, withWriteConsistency)
{
  zval *consistency = NULL;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",
                            &consistency) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  if (php_cassandra_get_consistency(consistency,
                                    &self->write_consistency TSRMLS_CC) == FAILURE) {
    return;
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseGraphOptionsBuilder, withRequestTimeout)
{
  zval *timeout;
  dse_graph_options_builder *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &timeout) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(timeout) != IS_DOUBLE || Z_DVAL_P(timeout) < 0) {
    INVALID_ARGUMENT(timeout, "a positive number");
  }

  self = PHP_DSE_GET_GRAPH_OPTIONS_BUILDER(getThis());
  self->request_timeout = ceil(Z_DVAL_P(timeout) * 1000);

  RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_graph_language, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, language)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_graph_source, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_graph_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_graph_consistency, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, consistency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_graph_request_timeout, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, request_timeout)
ZEND_END_ARG_INFO()

static zend_function_entry dse_graph_options_builder_methods[] = {
  PHP_ME(DseGraphOptionsBuilder, build, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withGraphLanguage, arginfo_graph_language, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withGraphSource, arginfo_graph_source, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withGraphName, arginfo_graph_name, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withReadConsistency, arginfo_graph_consistency, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withWriteConsistency, arginfo_graph_consistency, ZEND_ACC_PUBLIC)
  PHP_ME(DseGraphOptionsBuilder, withRequestTimeout, arginfo_graph_request_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_graph_options_builder_handlers;

static HashTable *
php_dse_graph_options_builder_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_graph_options_builder_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_graph_options_builder_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_graph_options_builder *self = PHP5TO7_ZEND_OBJECT_GET(dse_graph_options_builder, object);

  if (self->graph_language) {
    efree(self->graph_language);
  }
  if (self->graph_source) {
    efree(self->graph_source);
  }
  if (self->graph_name) {
    efree(self->graph_name);
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_graph_options_builder_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_graph_options_builder *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_graph_options_builder, ce);

  self->graph_language = NULL;
  self->graph_source = NULL;
  self->graph_name = NULL;
  self->read_consistency = CASS_CONSISTENCY_UNKNOWN;
  self->write_consistency = CASS_CONSISTENCY_UNKNOWN;
  self->request_timeout = -1;

  PHP5TO7_ZEND_OBJECT_INIT(dse_graph_options_builder, self, ce);
}

void dse_define_GraphOptionsBuilder(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Graph\\Options\\Builder", dse_graph_options_builder_methods);
  dse_graph_options_builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_graph_options_builder_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_graph_options_builder_ce->create_object = php_dse_graph_options_builder_new;

  memcpy(&dse_graph_options_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_graph_options_builder_handlers.get_properties  = php_dse_graph_options_builder_properties;
  dse_graph_options_builder_handlers.compare_objects = php_dse_graph_options_builder_compare;
}
