/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"

// For php_driver_value_compare
#include "util/hash.h"

#include "util/types.h"

#include "LineString.h"
#include "Point.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *php_driver_line_string_ce = NULL;

static DseLineString*
build_line_string(php_driver_line_string* line_string TSRMLS_DC)
{
  DseLineString *result = php_driver_line_string_g(TSRMLS_C);
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(points, current) {
    php_driver_point *point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    ASSERT_SUCCESS_BLOCK(dse_line_string_add_point(result, point->x, point->y),
        return NULL;
    );
  } PHP5TO7_ZEND_HASH_FOREACH_END(points);

  ASSERT_SUCCESS_BLOCK(dse_line_string_finish(result),
      return NULL;
  );

  return result;
}

#define EXPAND_PARAMS(line_string) build_line_string(line_string TSRMLS_CC)
PHP_DRIVER_DEFINE_DSE_TYPE_HELPERS(line_string, LINE_STRING, EXPAND_PARAMS)
#undef EXPAND_PARAMS

int php_driver_line_string_construct_from_value(const CassValue *value,
                                                php5to7_zval *out TSRMLS_DC)
{
  DseLineStringIterator* iterator = PHP_DRIVER_G(iterator_line_string);
  ASSERT_SUCCESS_BLOCK(dse_line_string_iterator_reset(iterator, value),
      return FAILURE;
  );

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), php_driver_line_string_ce);

  return php_driver_line_string_construct_from_iterator(iterator,
                                                        PHP5TO7_ZVAL_MAYBE_DEREF(out) TSRMLS_CC);
}

char *php_driver_line_string_to_wkt(php_driver_line_string *line_string TSRMLS_DC)
{
  char *result = NULL;
  char coord_buf[100];
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  if (zend_hash_num_elements(points) > 0) {
    // Traverse the points array and iteratively build "wkt".

    php5to7_ulong num_key;
    php5to7_zval *point_obj;
    char *temp_string;
    smart_str wkt = PHP5TO7_SMART_STR_INIT;
    smart_str_appends(&wkt, "LINESTRING (");

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(points, num_key, point_obj) {
      php_driver_point *point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(point_obj));

      if (num_key > 0) {
        // Add the comma that separates points in the wkt.
        smart_str_appends(&wkt, ", ");
      }

      // Add the next point in the form "x y" for wkt.
      sprintf(coord_buf, COORD_FMT, point->x);
      smart_str_appends(&wkt, coord_buf);
      smart_str_appends(&wkt, " ");
      sprintf(coord_buf, COORD_FMT, point->y);
      smart_str_appends(&wkt, coord_buf);
    } PHP5TO7_ZEND_HASH_FOREACH_END(points);

    // Wrap up the wkt.
    smart_str_appendc(&wkt, ')');
    smart_str_0(&wkt);
    temp_string = PHP5TO7_SMART_STR_VAL(wkt);
    result = ecalloc(PHP5TO7_SMART_STR_LEN(wkt) + 1, 1);
    strncpy(result, temp_string, PHP5TO7_SMART_STR_LEN(wkt));
    smart_str_free(&wkt);
  } else {
    spprintf(&result, 0, "%s", "LINESTRING EMPTY");
  }

  return result;
}

char *php_driver_line_string_to_string(php_driver_line_string *line_string TSRMLS_DC)
{
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);
  char* result = NULL;
  if (zend_hash_num_elements(points) > 0) {
    // Traverse the points array and iteratively build the to-string rep.

    php5to7_ulong num_key;
    php5to7_zval *point_obj;
    char *point_string;
    char *temp_string;
    smart_str to_string = PHP5TO7_SMART_STR_INIT;

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(points, num_key, point_obj) {
      php_driver_point *point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(point_obj));

      if (num_key > 0) {
        // Add the " to " that separates points in toString.
        smart_str_appends(&to_string, " to ");
      }

      // Add the next point to to_string.
      point_string = php_driver_point_to_string(point);
      smart_str_appends(&to_string, point_string);
      efree(point_string);
    } PHP5TO7_ZEND_HASH_FOREACH_END(points);

    // Wrap up the toString representation.
    smart_str_0(&to_string);
    temp_string = PHP5TO7_SMART_STR_VAL(to_string);
    result = ecalloc(PHP5TO7_SMART_STR_LEN(to_string) + 1, 1);
    strncpy(result, temp_string, PHP5TO7_SMART_STR_LEN(to_string));
    smart_str_free(&to_string);
  } else {
    result = emalloc(1);
    *result = '\0';
  }
  return result;
}

int php_driver_line_string_construct_from_iterator(DseLineStringIterator* iterator,
                                                   zval *return_value TSRMLS_DC)
{
  php_driver_line_string *line_string;
  size_t i, num_points;
  HashTable *points;

  line_string = PHP_DRIVER_GET_LINE_STRING(return_value);
  points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  num_points = dse_line_string_iterator_num_points(iterator);

  for (i = 0; i < num_points; ++i) {
    php5to7_zval zpoint;
    php_driver_point *point;

    PHP5TO7_ZVAL_MAYBE_MAKE(zpoint);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zpoint), php_driver_point_ce);
    point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_P(zpoint));
    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(points,
                                        PHP5TO7_ZVAL_MAYBE_P(zpoint),
                                        sizeof(zval *));

    ASSERT_SUCCESS_VALUE(dse_line_string_iterator_next_point(iterator, &point->x, &point->y),
                         FAILURE);
  }

  return SUCCESS;
}

PHP_METHOD(LineString, __construct)
{
  php5to7_zval_args args;
  php_driver_line_string *self = NULL;
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  if (num_args == 1) {
    zval *point_obj_or_wkt = PHP5TO7_ZVAL_ARG(args[0]);

    if (Z_TYPE_P(point_obj_or_wkt) == IS_STRING) {
      int rc;

      // Create and initialize a "text iterator" which parses wkt. If there's a parse
      // error, it typically occurs during initialization. However, it's theoretically
      // possible to run into an issue when traversing the iterator, so cover that, too.

      DseLineStringIterator* iterator = PHP_DRIVER_G(iterator_line_string);
      rc = dse_line_string_iterator_reset_with_wkt_n(iterator, Z_STRVAL_P(point_obj_or_wkt), Z_STRLEN_P(point_obj_or_wkt));
      if (rc == CASS_OK) {
        rc = php_driver_line_string_construct_from_iterator(iterator, getThis() TSRMLS_CC);
      }

      if (rc != SUCCESS && rc != CASS_OK) {
        // Failed to parse the wkt properly. Yell.
        throw_invalid_argument(point_obj_or_wkt, "Argument 1", "valid WKT for a LineString" TSRMLS_CC);
      }

      PHP5TO7_MAYBE_EFREE(args);
      return;
    }
  }

  if (num_args > 0) {
    int i;

    if (num_args == 1) {
      // A LineString must have at least two points!
      zend_throw_exception_ex(spl_ce_BadFunctionCallException, 0 TSRMLS_CC, "%s",
                              "A line-string must have at least two points or be empty"
                              );
      PHP5TO7_MAYBE_EFREE(args);
      return;
    }

    // Every arg must be a Point.
    for (i = 0; i < num_args; ++i) {
      zval* point_obj = PHP5TO7_ZVAL_ARG(args[i]);
      if (Z_TYPE_P(point_obj) != IS_OBJECT || Z_OBJCE_P(point_obj) != php_driver_point_ce) {
        char *object_name;
        spprintf(&object_name, 0, "Argument %d", i+1);
        throw_invalid_argument(point_obj, object_name, "an instance of " PHP_DRIVER_NAMESPACE "\\Point" TSRMLS_CC);
        efree(object_name);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
    }

    // Populate the points array based on args.
    self = PHP_DRIVER_GET_LINE_STRING(getThis());
    for (i = 0; i < num_args; ++i) {
      zval* point_obj = PHP5TO7_ZVAL_ARG(args[i]);

      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(self->points), point_obj);
      Z_TRY_ADDREF_P(point_obj);
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

PHP_METHOD(LineString, __toString)
{
  php_driver_line_string *self = NULL;
  char *to_string = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_LINE_STRING(getThis());
  to_string = php_driver_line_string_to_string(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(to_string);
  efree(to_string);
}

PHP_METHOD(LineString, type)
{
  php_driver_line_string_type(return_value TSRMLS_CC);
}

PHP_METHOD(LineString, points)
{
  php_driver_line_string *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_LINE_STRING(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->points), 1, 0);
}

PHP_METHOD(LineString, point)
{
  ulong index;
  php_driver_line_string *self = NULL;
  php5to7_zval *value;
  HashTable *points;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_LINE_STRING(getThis());

  points = PHP5TO7_Z_ARRVAL_MAYBE_P(self->points);

  if (PHP5TO7_ZEND_HASH_INDEX_FIND(points, index, value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(LineString, wkt)
{
  php_driver_line_string *self = NULL;
  char *wkt = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_LINE_STRING(getThis());
  wkt = php_driver_line_string_to_wkt(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(wkt);
  efree(wkt);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, points)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_point, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_line_string_methods[] = {
  PHP_ME(LineString, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(LineString, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(LineString, points, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(LineString, point, arginfo_point, ZEND_ACC_PUBLIC)
  PHP_ME(LineString, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(LineString, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_line_string_handlers;

static HashTable *
php_driver_line_string_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  php_driver_line_string *self = PHP_DRIVER_GET_LINE_STRING(object);

  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->points));
  PHP5TO7_ZEND_HASH_UPDATE(props, "points", sizeof("points"), PHP5TO7_ZVAL_MAYBE_P(self->points), sizeof(zval));

  return props;
}

static int
php_driver_line_string_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_line_string *left;
  php_driver_line_string *right;
  HashTable *left_points;
  HashTable *right_points;
  unsigned left_num_points;
  unsigned right_num_points;
  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;
  int result;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  left = PHP_DRIVER_GET_LINE_STRING(obj1);
  right = PHP_DRIVER_GET_LINE_STRING(obj2);

  left_points = PHP5TO7_Z_ARRVAL_MAYBE_P(left->points);
  right_points = PHP5TO7_Z_ARRVAL_MAYBE_P(right->points);
  left_num_points = zend_hash_num_elements(left_points);
  right_num_points = zend_hash_num_elements(right_points);

  // The line-string with fewer points is the "lesser" of the two.
  if (left_num_points != right_num_points) {
    return left_num_points < right_num_points ? -1 : 1;
  }

  zend_hash_internal_pointer_reset_ex(left_points, &pos1);
  zend_hash_internal_pointer_reset_ex(right_points, &pos2);

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(left_points, current1, &pos1) &&
         PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(right_points, current2, &pos2)) {
    result = php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                      PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (result != 0) return result;
    zend_hash_move_forward_ex(left_points, &pos1);
    zend_hash_move_forward_ex(right_points, &pos2);
  }

  return 0;
}

static unsigned
php_driver_line_string_hash_value(zval *obj TSRMLS_DC)
{
  php5to7_zval *current;
  php_driver_line_string *self = PHP_DRIVER_GET_LINE_STRING(obj);
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(self->points);
  unsigned hashv = 0;

  PHP5TO7_ZEND_HASH_FOREACH_VAL(points, current) {
    hashv = php_driver_combine_hash(hashv, php_driver_value_hash(PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC));
  } PHP5TO7_ZEND_HASH_FOREACH_END(points);

  return hashv;
}

static void
php_driver_line_string_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_line_string *self = PHP5TO7_ZEND_OBJECT_GET(line_string, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->points);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_line_string_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_line_string *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(line_string, ce);

  PHP5TO7_ZVAL_MAYBE_MAKE(self->points);
  array_init(PHP5TO7_ZVAL_MAYBE_P(self->points));

  PHP5TO7_ZEND_OBJECT_INIT(line_string, self, ce);
}

void php_driver_define_LineString(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\LineString", php_driver_line_string_methods);
  php_driver_line_string_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_custom_ce);
  php_driver_line_string_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_line_string_ce->create_object = php_driver_line_string_new;

  memcpy(&php_driver_line_string_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_line_string_handlers.std.get_properties  = php_driver_line_string_properties;
  php_driver_line_string_handlers.std.compare_objects = php_driver_line_string_compare;
  php_driver_line_string_handlers.std.clone_obj = NULL;

  php_driver_line_string_handlers.hash_value = php_driver_line_string_hash_value;

}
