#include "php_dse.h"
#include "php_dse_globals.h"
#include "php_dse_types.h"
#include "Point.h"

// For php_cassandra_value_compare
#include "util/hash.h"

#include "util/types.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

#define DSE_LINE_STRING_TYPE "org.apache.cassandra.db.marshal.LineStringType"

zend_class_entry *dse_line_string_ce = NULL;

static DseLineString*
build_line_string(HashTable *points TSRMLS_DC)
{
  DseLineString *result = dse_line_string_new();

  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(points, current) {
    dse_point *point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    ASSERT_SUCCESS_BLOCK(dse_line_string_add_point(result, point->x, point->y),
                         dse_line_string_free(result);
                         return NULL;
    );
  } PHP5TO7_ZEND_HASH_FOREACH_END(points);

  ASSERT_SUCCESS_BLOCK(dse_line_string_finish(result),
                       dse_line_string_free(result);
                       return NULL;
  );

  return result;
}

static int
marshal_bind_by_index(CassStatement *statement, size_t index, zval *value TSRMLS_DC)
{
  dse_line_string *line_string = PHP_DSE_GET_LINE_STRING(value);
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  DseLineString *dse_line_string = build_line_string(points TSRMLS_CC);

  ASSERT_SUCCESS_BLOCK(cass_statement_bind_dse_line_string(statement,
                                                           index,
                                                           dse_line_string),
                       dse_line_string_free(dse_line_string);
                       return FAILURE;
  );

  dse_line_string_free(dse_line_string);
  return SUCCESS;
}

static int
marshal_bind_by_name(CassStatement *statement, const char *name, zval *value TSRMLS_DC)
{
  dse_line_string *line_string = PHP_DSE_GET_LINE_STRING(value);
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  DseLineString *dse_line_string = build_line_string(points TSRMLS_CC);

  ASSERT_SUCCESS_BLOCK(cass_statement_bind_dse_line_string_by_name(statement,
                                                           name,
                                                           dse_line_string),
                       dse_line_string_free(dse_line_string);
                       return FAILURE;
  );

  dse_line_string_free(dse_line_string);
  return SUCCESS;
}

static int
marshal_get_result(const CassValue *value, php5to7_zval *out TSRMLS_DC)
{
  dse_line_string *line_string;
  size_t i, num_points;
  HashTable *points;
  DseLineStringIterator* iterator = dse_line_string_iterator_new();

  ASSERT_SUCCESS_BLOCK(dse_line_string_iterator_reset(iterator, value),
                       dse_line_string_iterator_free(iterator);
                       return FAILURE;
  );

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), dse_line_string_ce);
  line_string = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(out));
  points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  num_points = dse_line_string_iterator_num_points(iterator);

  for (i = 0; i < num_points; ++i) {
    php5to7_zval zpoint;
    dse_point *point;

    PHP5TO7_ZVAL_MAYBE_MAKE(zpoint);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zpoint), dse_point_ce);
    point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_P(zpoint));
    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(points,
                                        PHP5TO7_ZVAL_MAYBE_P(zpoint),
                                        sizeof(zval *));

    ASSERT_SUCCESS_BLOCK(dse_line_string_iterator_next_point(iterator, &point->x, &point->y),
                         dse_line_string_iterator_free(iterator);
                         return FAILURE;
    );
  }

  return SUCCESS;
}

char *line_string_to_wkt(dse_line_string *line_string TSRMLS_DC)
{
  char *result = NULL;
  char coord_buf[100];
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

  if (zend_hash_num_elements(points) > 0) {
    // Traverse the points array and iteratively build "wkt".

    php5to7_ulong num_key;
    php5to7_zval *point_obj;
    smart_str wkt = PHP5TO7_SMART_STR_INIT;
    smart_str_appends(&wkt, "LINESTRING (");

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(points, num_key, point_obj) {
      dse_point *point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(point_obj));

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
    result = ecalloc(PHP5TO7_SMART_STR_LEN(wkt) + 1, 1);
    strncpy(result, PHP5TO7_SMART_STR_VAL(wkt), PHP5TO7_SMART_STR_LEN(wkt));
    smart_str_free(&wkt);
  } else {
    spprintf(&result, 0, "%s", "LINESTRING EMPTY");
  }

  return result;
}

char *line_string_to_string(dse_line_string *line_string TSRMLS_DC)
{
  HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);
  char* result = NULL;
  if (zend_hash_num_elements(points) > 0) {
    // Traverse the points array and iteratively build the to-string rep.

    php5to7_ulong num_key;
    php5to7_zval *point_obj;
    char *point_string;
    smart_str to_string = PHP5TO7_SMART_STR_INIT;

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(points, num_key, point_obj) {
      dse_point *point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(point_obj));

      if (num_key > 0) {
        // Add the " to " that separates points in toString.
        smart_str_appends(&to_string, " to ");
      }

      // Add the next point to to_string.
      point_string = point_to_string(point);
      smart_str_appends(&to_string, point_string);
      efree(point_string);
    } PHP5TO7_ZEND_HASH_FOREACH_END(points);

    // Wrap up the toString representation.
    smart_str_0(&to_string);
    result = ecalloc(PHP5TO7_SMART_STR_LEN(to_string) + 1, 1);
    strncpy(result, PHP5TO7_SMART_STR_VAL(to_string), PHP5TO7_SMART_STR_LEN(to_string));
    smart_str_free(&to_string);
  } else {
    result = emalloc(1);
    *result = '\0';
  }
  return result;
}

PHP_METHOD(DseLineString, __construct)
{
  php5to7_zval_args point_args;
  dse_line_string *self = PHP_DSE_GET_LINE_STRING(getThis());
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &point_args, &num_args) == FAILURE) {
    return;
  }

  if (num_args > 0) {
    // Populate the points array based on args. Since we're traversing each point, iteratively construct to
    // "string" and "wkt" members using PHP smart strings.


    for (int i = 0; i < num_args; ++i) {
      zval* point_obj = PHP5TO7_ZVAL_ARG(point_args[i]);

      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(self->points), point_obj);
      Z_TRY_ADDREF_P(point_obj);
    }

    PHP5TO7_MAYBE_EFREE(point_args);
  }
}

PHP_METHOD(DseLineString, __toString)
{
  dse_line_string *self = NULL;
  char *to_string = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  to_string = line_string_to_string(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(to_string);
  efree(to_string);
}

PHP_METHOD(DseLineString, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(DSE_G(type_line_string))) {
    DSE_G(type_line_string) = php_cassandra_type_custom(DSE_LINE_STRING_TYPE TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(DSE_G(type_line_string)), 1, 0);
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
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(DseLineString, wkt)
{
  dse_line_string *self = NULL;
  char *wkt = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_LINE_STRING(getThis());
  wkt = line_string_to_wkt(self TSRMLS_CC);
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

static zend_function_entry dse_line_string_methods[] = {
  PHP_ME(DseLineString, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DseLineString, type, arginfo_none, ZEND_ACC_PUBLIC)
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
  dse_line_string *self = PHP_DSE_GET_LINE_STRING(object);

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

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->points);

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

  PHP5TO7_ZEND_OBJECT_INIT(dse_line_string, self, ce);
}

void dse_define_LineString(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\LineString", dse_line_string_methods);
  dse_line_string_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_line_string_ce TSRMLS_CC, 1, cassandra_custom_ce);
  dse_line_string_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_line_string_ce->create_object = php_dse_line_string_new;

  memcpy(&dse_line_string_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_line_string_handlers.get_properties  = php_dse_line_string_properties;
  dse_line_string_handlers.compare_objects = php_dse_line_string_compare;
  dse_line_string_handlers.clone_obj = NULL;

  php_cassandra_custom_marshal_add(DSE_LINE_STRING_TYPE,
                                   marshal_bind_by_index,
                                   marshal_bind_by_name,
                                   marshal_get_result TSRMLS_CC);
}
