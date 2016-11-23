#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_line_string_ce = NULL;

// Helper function to copy the points collection out of a line-string struct
// to a previously initialized array.
static void
copy_points(dse_line_string *line_string, zval *array)
{
  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(&line_string->points, current) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_DEREF(current)) == SUCCESS)
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    else
      break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(&line_string->points);
}

static int
to_string(zval *result, dse_line_string *line_string TSRMLS_DC)
{
  char *string;
  spprintf(&string, 0, "__NOT_IMPLEMENTED__");
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
  return SUCCESS;
}

PHP_METHOD(DseLineString, __construct)
{
  php5to7_zval_args point_args;
  dse_line_string *line_string = NULL;
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &point_args, &num_args) == FAILURE) {
    return;
  }

  if (num_args > 0) {
    line_string = PHP_DSE_GET_LINE_STRING(getThis());
    for (int i = 0; i < num_args; ++i) {
      zval* point = PHP5TO7_ZVAL_ARG(point_args[i]);
      if (PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(&line_string->points, point, sizeof(zval *))) {
        Z_TRY_ADDREF_P(point);
      }
    }

    PHP5TO7_MAYBE_EFREE(point_args);
  }
}

PHP_METHOD(DseLineString, __toString)
{
  dse_line_string *self = PHP_DSE_GET_LINE_STRING(getThis());

  to_string(return_value, self TSRMLS_CC);
}

PHP_METHOD(DseLineString, points)
{
  dse_line_string *line_string = NULL;
  array_init(return_value);
  line_string = PHP_DSE_GET_LINE_STRING(getThis());
  copy_points(line_string, return_value);
}

PHP_METHOD(DseLineString, wkt)
{
  dse_line_string *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->wkt)) {
    char* rep;
    spprintf(&rep, 0, "__NOT_IMPLEMENTED__");
    PHP5TO7_ZVAL_MAYBE_MAKE(self->wkt);
    PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), rep, strlen(rep));
    efree(rep);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, points)
ZEND_END_ARG_INFO()

static zend_function_entry dse_line_string_methods[] = {
  PHP_ME(DseLineString, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DseLineString, points, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseLineString, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseLineString, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_line_string_handlers;

static HashTable *
php_dse_line_string_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_line_string_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_line_string_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_line_string *self = PHP5TO7_ZEND_OBJECT_GET(dse_line_string, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_line_string_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_line_string *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_line_string, ce);

  zend_hash_init(&self->points, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT(dse_line_string, self, ce);
}

void dse_define_LineString(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\LineString", dse_line_string_methods);
  dse_line_string_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_line_string_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_line_string_ce->create_object = php_dse_line_string_new;

  memcpy(&dse_line_string_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_line_string_handlers.get_properties  = php_dse_line_string_properties;
  dse_line_string_handlers.compare_objects = php_dse_line_string_compare;
  dse_line_string_handlers.clone_obj = NULL;
}
