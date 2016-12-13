#include "php_dse.h"
#include "php_dse_globals.h"
#include "php_dse_types.h"

#include "util/types.h"

zend_class_entry *dse_point_ce = NULL;

#define DSE_POINT_TYPE "org.apache.cassandra.db.marshal.PointType"

static int
marshal_bind_by_index(CassStatement *statement, size_t index, zval *value TSRMLS_DC)
{
  dse_point *point = PHP_DSE_GET_POINT(value);
  ASSERT_SUCCESS_VALUE(cass_statement_bind_dse_point(statement,
                                                     index,
                                                     point->x, point->y),
                       FAILURE);
  return SUCCESS;
}

static int
marshal_bind_by_name(CassStatement *statement, const char *name, zval *value TSRMLS_DC)
{
  dse_point *point = PHP_DSE_GET_POINT(value);
  ASSERT_SUCCESS_VALUE(cass_statement_bind_dse_point_by_name(statement,
                                                             name,
                                                             point->x, point->y),
                       FAILURE);
  return SUCCESS;
}

static int
marshal_get_result(const CassValue *value, php5to7_zval *out TSRMLS_DC)
{
  dse_point *point;

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), dse_point_ce);
  point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(out));

  ASSERT_SUCCESS_VALUE(cass_value_get_dse_point(value, &point->x, &point->y),
                       FAILURE);

  return SUCCESS;
}

char *point_to_wkt(dse_point *point)
{
  char* rep;
  spprintf(&rep, 0, "POINT (" COORD_FMT " " COORD_FMT ")", point->x, point->y);
  return rep;
}

char *point_to_string(dse_point *point)
{
  char* rep;
  spprintf(&rep, 0, COORD_FMT "," COORD_FMT, point->x, point->y);
  return rep;
}

PHP_METHOD(DsePoint, __construct)
{
  double coord[2];
  php5to7_zval_args args;
  dse_point *self = NULL;
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
  self = PHP_DSE_GET_POINT(getThis());
  self->x = coord[0];
  self->y = coord[1];
  PHP5TO7_MAYBE_EFREE(args);
}

PHP_METHOD(DsePoint, __toString)
{
  char* rep;
  dse_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POINT(getThis());

  // Build up string representation of this point.
  rep = point_to_string(self);
  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

PHP_METHOD(DsePoint, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(DSE_G(type_point))) {
    DSE_G(type_point) = php_cassandra_type_custom(DSE_POINT_TYPE TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(DSE_G(type_point)), 1, 0);
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
  char* rep;
  dse_point *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POINT(getThis());

  // Build up wkt representation of this point.
  rep = point_to_wkt(self);
  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, point_spec)
ZEND_END_ARG_INFO()

static zend_function_entry dse_point_methods[] = {
  PHP_ME(DsePoint, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DsePoint, type, arginfo_none, ZEND_ACC_PUBLIC)
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
  dse_point  *self = PHP_DSE_GET_POINT(object);

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
php_dse_point_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  dse_point  *left = PHP_DSE_GET_POINT(obj1);
  dse_point  *right = PHP_DSE_GET_POINT(obj2);

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
  dse_point *self = PHP5TO7_ZEND_OBJECT_ECALLOC(dse_point, ce);
  PHP5TO7_ZEND_OBJECT_INIT(dse_point, self, ce);
}

void dse_define_Point(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Point", dse_point_methods);
  dse_point_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_point_ce TSRMLS_CC, 1, cassandra_custom_ce);
  dse_point_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_point_ce->create_object = php_dse_point_new;

  memcpy(&dse_point_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_point_handlers.get_properties  = php_dse_point_properties;
  dse_point_handlers.compare_objects = php_dse_point_compare;
  dse_point_handlers.clone_obj = NULL;

  php_cassandra_custom_marshal_add(DSE_POINT_TYPE,
                                   marshal_bind_by_index,
                                   marshal_bind_by_name,
                                   marshal_get_result TSRMLS_CC);
}
