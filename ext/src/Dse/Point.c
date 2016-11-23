#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_point_ce = NULL;

static int
to_string(zval *result, dse_point *point TSRMLS_DC)
{
  char *string;
  spprintf(&string, 0, "%f, %f", point->x, point->y);
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
  return SUCCESS;
}

PHP_METHOD(DsePoint, __construct)
{
  double x;
  double y;
  dse_point *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &x, &y) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_POINT(getThis());
  self->x = x;
  self->y = y;
}

PHP_METHOD(DsePoint, __toString)
{
  dse_point *self = PHP_DSE_GET_POINT(getThis());

  to_string(return_value, self TSRMLS_CC);
}

PHP_METHOD(DsePoint, x)
{
  dse_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POINT(getThis());
  RETURN_DOUBLE(self->x);
}

PHP_METHOD(DsePoint, y)
{
  dse_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POINT(getThis());
  RETURN_DOUBLE(self->y);
}

PHP_METHOD(DsePoint, wkt)
{
  dse_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POINT(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->wkt)) {
    char* rep;
    spprintf(&rep, 0, "POINT (%f %f)", self->x, self->y);
    PHP5TO7_ZVAL_MAYBE_MAKE(self->wkt);
    PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), rep, strlen(rep));
    efree(rep);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, x)
  ZEND_ARG_INFO(0, y)
ZEND_END_ARG_INFO()

static zend_function_entry dse_point_methods[] = {
  PHP_ME(DsePoint, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DsePoint, x, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePoint, y, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePoint, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePoint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_point_handlers;

static HashTable *
php_dse_point_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  /* TODO Remove if we don't plan to expose attributes as properties. Fix if we do; current impl is
     copying the value, and the attribute seems writable. Also, there's a masking issue with the method.
  dse_point  *self = PHP_DSE_GET_POINT(object);

  php5to7_zval wrappedX;
  PHP5TO7_ZVAL_MAYBE_MAKE(wrappedX);
  ZVAL_DOUBLE(PHP5TO7_ZVAL_MAYBE_P(wrappedX), self->x);
  PHP5TO7_ZEND_HASH_UPDATE(props, "x", sizeof("x"), PHP5TO7_ZVAL_MAYBE_P(wrappedX), sizeof(zval));
*/
  return props;
}

static int
php_dse_point_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_point_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_point *self = PHP5TO7_ZEND_OBJECT_GET(dse_point, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_point_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_point *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_point, ce);

  /* Initialize */
  PHP5TO7_ZEND_OBJECT_INIT(dse_point, self, ce);
}

void dse_define_Point(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Point", dse_point_methods);
  dse_point_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_point_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_point_ce->create_object = php_dse_point_new;

  memcpy(&dse_point_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_point_handlers.get_properties  = php_dse_point_properties;
  dse_point_handlers.compare_objects = php_dse_point_compare;
  dse_point_handlers.clone_obj = NULL;
}
