#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"

#include "util/types.h"

#include "Point.h"

zend_class_entry *php_driver_point_ce = NULL;

int php_driver_point_bind_by_index(CassStatement *statement,
                                   size_t index,
                                   zval *value TSRMLS_DC)
{
  php_driver_point *point = PHP_DRIVER_GET_POINT(value);
  ASSERT_SUCCESS_VALUE(cass_statement_bind_dse_point(statement,
                                                     index,
                                                     point->x, point->y),
                       FAILURE);
  return SUCCESS;
}

int php_driver_point_bind_by_name(CassStatement *statement,
                                  const char *name,
                                  zval *value TSRMLS_DC)
{
  php_driver_point *point = PHP_DRIVER_GET_POINT(value);
  ASSERT_SUCCESS_VALUE(cass_statement_bind_dse_point_by_name(statement,
                                                             name,
                                                             point->x, point->y),
                       FAILURE);
  return SUCCESS;
}

int php_driver_point_construct_from_value(const CassValue *value,
                                          php5to7_zval *out TSRMLS_DC)
{
  php_driver_point *point;

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), php_driver_point_ce);
  point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(out));

  ASSERT_SUCCESS_VALUE(cass_value_get_dse_point(value, &point->x, &point->y),
                       FAILURE);

  return SUCCESS;
}

char *php_driver_point_to_wkt(php_driver_point *point)
{
  char* rep;
  spprintf(&rep, 0, "POINT (" COORD_FMT " " COORD_FMT ")", point->x, point->y);
  return rep;
}

char *php_driver_point_to_string(php_driver_point *point)
{
  char* rep;
  spprintf(&rep, 0, COORD_FMT "," COORD_FMT, point->x, point->y);
  return rep;
}

PHP_METHOD(Point, __construct)
{
  double coord[2];
  php5to7_zval_args args;
  php_driver_point *self = NULL;
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  if (num_args == 1) {
    // We accept one arg *only* if its a WKT string.
    zval *wkt_zval = PHP5TO7_ZVAL_ARG(args[0]);

    // coord is a 2-element array, for x,y. We want to parse the WKT string and populate
    // the array.
    if (Z_TYPE_P(wkt_zval) != IS_STRING ||
        dse_point_from_wkt_n(Z_STRVAL_P(wkt_zval), Z_STRLEN_P(wkt_zval), coord, coord + 1) != CASS_OK) {
      // The arg isn't a string/wkt; yell.
      throw_invalid_argument(wkt_zval, "Argument 1", "valid WKT for a Point" TSRMLS_CC);
      PHP5TO7_MAYBE_EFREE(args);
      return;
    }
  } else if (num_args == 2) {
    // With two args, they must be numeric.
    int i = 0;

    // We'll replace the last space with the arg num if there's an error.
    char arg_descriptor[] = "Argument  ";
    for ( ; i < 2; ++i) {
      zval *arg = PHP5TO7_ZVAL_ARG(args[i]);
      if (Z_TYPE_P(arg) == IS_LONG) {
        coord[i] = Z_LVAL_P(arg);
      } else if (Z_TYPE_P(arg) == IS_DOUBLE) {
        coord[i] = Z_DVAL_P(arg);
      } else {
        // We don't support this type.
        arg_descriptor[9] = '1' + i;
        throw_invalid_argument(arg, arg_descriptor, "a long or a double" TSRMLS_CC);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
    }
  } else {
    // Invalid number of args.
    zend_throw_exception_ex(spl_ce_BadFunctionCallException, 0 TSRMLS_CC,
                            "A Point may only be constructed with 1 string argument (WKT) or 2 numbers (x,y)"
                            );
    if (num_args > 0) {
      PHP5TO7_MAYBE_EFREE(args);
    }
    return;
  }

  // If we get here, we've processed args and are happy.
  self = PHP_DRIVER_GET_POINT(getThis());
  self->x = coord[0];
  self->y = coord[1];
  PHP5TO7_MAYBE_EFREE(args);
}

PHP_METHOD(Point, __toString)
{
  char* rep;
  php_driver_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POINT(getThis());

  // Build up string representation of this point.
  rep = php_driver_point_to_string(self);
  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

PHP_METHOD(Point, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(PHP_DRIVER_G(type_point))) {
    PHP_DRIVER_G(type_point) = php_driver_type_custom(DSE_POINT_TYPE TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(PHP_DRIVER_G(type_point)), 1, 0);
}

PHP_METHOD(Point, x)
{
  php_driver_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POINT(getThis());
  RETURN_DOUBLE(self->x);
}

PHP_METHOD(Point, y)
{
  php_driver_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POINT(getThis());
  RETURN_DOUBLE(self->y);
}

PHP_METHOD(Point, wkt)
{
  char* rep;
  php_driver_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POINT(getThis());

  // Build up wkt representation of this point.
  rep = php_driver_point_to_wkt(self);
  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, point_spec)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_point_methods[] = {
  PHP_ME(Point, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Point, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Point, x, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Point, y, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Point, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Point, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_point_handlers;

static HashTable *
php_driver_point_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  php_driver_point  *self = PHP_DRIVER_GET_POINT(object);

  php5to7_zval wrappedX, wrappedY;
  PHP5TO7_ZVAL_MAYBE_MAKE(wrappedX);
  PHP5TO7_ZVAL_MAYBE_MAKE(wrappedY);
  ZVAL_DOUBLE(PHP5TO7_ZVAL_MAYBE_P(wrappedX), self->x);
  ZVAL_DOUBLE(PHP5TO7_ZVAL_MAYBE_P(wrappedY), self->y);
  PHP5TO7_ZEND_HASH_UPDATE(props, "x", sizeof("x"), PHP5TO7_ZVAL_MAYBE_P(wrappedX), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "y", sizeof("y"), PHP5TO7_ZVAL_MAYBE_P(wrappedY), sizeof(zval));

  return props;
}

static int
php_driver_point_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  php_driver_point  *left = PHP_DRIVER_GET_POINT(obj1);
  php_driver_point  *right = PHP_DRIVER_GET_POINT(obj2);

  // Comparisons compare x, then y.

  // left has smaller x.
  if (left->x < right->x)
    return -1;

  if (left->x == right->x) {
    // x's are the same; compare y's.
    if (left->y < right->y)
      return -1;
    else if (left->y == right->y)
      return 0;
    else
      return 1;
  } else {
    // left x is larger than right x.
    return 1;
  }
}

static void
php_driver_point_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_point *self = PHP5TO7_ZEND_OBJECT_GET(point, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_point_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_point *self = PHP5TO7_ZEND_OBJECT_ECALLOC(point, ce);
  PHP5TO7_ZEND_OBJECT_INIT(point, self, ce);
}

void php_driver_define_Point(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Point", php_driver_point_methods);
  php_driver_point_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_point_ce TSRMLS_CC, 1, php_driver_custom_ce);
  php_driver_point_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_point_ce->create_object = php_driver_point_new;

  memcpy(&php_driver_point_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_point_handlers.get_properties  = php_driver_point_properties;
  php_driver_point_handlers.compare_objects = php_driver_point_compare;
  php_driver_point_handlers.clone_obj = NULL;
}
