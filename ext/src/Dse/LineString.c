#include "php_dse.h"
#include "php_dse_types.h"

// For php_cassandra_value_compare
#include "util/hash.h"

zend_class_entry *dse_line_string_ce = NULL;

PHP_METHOD(DseLineString, __construct)
{
  php5to7_zval_args point_args;
  dse_line_string *self = PHP_DSE_GET_LINE_STRING(getThis());
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &point_args, &num_args) == FAILURE) {
    return;
  }

  if (num_args > 0) {
    // Populate the points array based on args. Since we're traversing each point, use it
    // as an opportunity to get the lengths of the 'string' attributes of the points. We'll need
    // that to make our 'string' in a bit. Get the dse_point* objects while we're at it.
    // Also build up the cpp dse driver's DseLineString, which we'll use to generate our
    // wkt.

    int to_string_len = 0;
    dse_point **points = emalloc(sizeof(dse_point*) * num_args);

    DseLineString *cpp_line_string = dse_line_string_new();

    for (int i = 0; i < num_args; ++i) {
      zval* point_obj = PHP5TO7_ZVAL_ARG(point_args[i]);
      points[i] = PHP_DSE_GET_POINT(point_obj);
      dse_line_string_add_point(cpp_line_string, points[i]->x, points[i]->y);
      to_string_len += strlen(PHP5TO7_Z_STRVAL_MAYBE_P(points[i]->string));
      if (add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(self->points), point_obj) == SUCCESS)
        Z_TRY_ADDREF_P(point_obj);
      else
        break;
    }

    // to_string_len has the total length of all point-string lengths. Our to-string format is
    // actually: x1,y1 to x2,y2 to x3,y3 ...
    // So we have " to " between points; so there are num_args - 1 of those; and then we have
    // our null terminator.
    to_string_len += 4 * (num_args-1) + 1;

    // Ok, create a buffer for our to-string, fill it, then zvalify!
    // We use ecalloc to get 0 initialization.
    char *to_string = ecalloc(to_string_len, 1);
    for (int i = 0; i < num_args; ++i) {
      if (i > 0) {
        strcat(to_string, " to ");
      }
      strcat(to_string, PHP5TO7_Z_STRVAL_MAYBE_P(points[i]->string));
    }

    PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(self->string), to_string, to_string_len - 1);

    // Use the cpp dse driver's logic to create the wkt.
    char* wkt;
    dse_line_string_to_wkt(cpp_line_string, &wkt);

    PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), wkt, strlen(wkt));

    dse_line_string_free_wkt_string(wkt);
    dse_line_string_free(cpp_line_string);

    efree(to_string);
    efree(points);
    PHP5TO7_MAYBE_EFREE(point_args);
  } else {
    PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(self->string), "");
    PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(self->wkt), "LINESTRING EMPTY");
  }
}

PHP_METHOD(DseLineString, __toString)
{
  dse_line_string *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->string), 1, 0);
}

PHP_METHOD(DseLineString, points)
{
  dse_line_string *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->points), 1, 0);
}

PHP_METHOD(DseLineString, point)
{
  ulong index;
  dse_line_string *self = NULL;
  php5to7_zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_LINE_STRING(getThis());

  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(self->points);

  if (PHP5TO7_ZEND_HASH_INDEX_FIND(points, index, value)) {
    Z_TRY_ADDREF_P(value);
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(DseLineString, wkt)
{
  dse_line_string *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->wkt), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, points)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_point, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static zend_function_entry dse_line_string_methods[] = {
  PHP_ME(DseLineString, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DseLineString, points, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseLineString, point, arginfo_point, ZEND_ACC_PUBLIC)
  PHP_ME(DseLineString, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseLineString, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_line_string_handlers;

static HashTable *
php_dse_line_string_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  dse_line_string  *self = PHP_DSE_GET_LINE_STRING(object);

  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->points));
  PHP5TO7_ZEND_HASH_UPDATE(props, "points", sizeof("points"), PHP5TO7_ZVAL_MAYBE_P(self->points), sizeof(zval));

  return props;
}

static int
php_dse_line_string_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  dse_line_string *left = PHP_DSE_GET_LINE_STRING(obj1);
  dse_line_string *right = PHP_DSE_GET_LINE_STRING(obj2);

  HashTable *left_points = PHP5TO7_Z_ARRVAL_MAYBE_P(left->points);
  HashTable *right_points = PHP5TO7_Z_ARRVAL_MAYBE_P(right->points);
  unsigned left_num_points = zend_hash_num_elements(left_points);
  unsigned right_num_points = zend_hash_num_elements(right_points);

  // The line-string with fewer points is the "lesser" of the two.
  if (left_num_points != right_num_points) {
    return left_num_points < right_num_points ? -1 : 1;
  }

  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;
  int result;

  zend_hash_internal_pointer_reset_ex(left_points, &pos1);
  zend_hash_internal_pointer_reset_ex(right_points, &pos2);

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(left_points, current1, &pos1) &&
         PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(right_points, current2, &pos2)) {
    result = php_cassandra_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                         PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (result != 0) return result;
    zend_hash_move_forward_ex(left_points, &pos1);
    zend_hash_move_forward_ex(right_points, &pos2);
  }

  return 0;
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

  PHP5TO7_ZVAL_MAYBE_MAKE(self->points);
  array_init(PHP5TO7_ZVAL_MAYBE_P(self->points));

  PHP5TO7_ZVAL_MAYBE_MAKE(self->string);
  PHP5TO7_ZVAL_MAYBE_MAKE(self->wkt);

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
