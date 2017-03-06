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
#include "Polygon.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *php_driver_polygon_ce = NULL;

static DsePolygon *
build_polygon(HashTable *line_strings TSRMLS_DC)
{
  php5to7_zval *current1;
  DsePolygon *result = dse_polygon_new();

  PHP5TO7_ZEND_HASH_FOREACH_VAL(line_strings, current1) {
    php5to7_zval *current2;
    php_driver_line_string *line_string = PHP_DRIVER_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(current1));
    HashTable *points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);

    ASSERT_SUCCESS_BLOCK(dse_polygon_start_ring(result),
                         dse_polygon_free(result);
                         return NULL;
    );

    PHP5TO7_ZEND_HASH_FOREACH_VAL(points, current2) {
      php_driver_point *point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(current2));
      ASSERT_SUCCESS_BLOCK(dse_polygon_add_point(result, point->x, point->y),
                           dse_polygon_free(result);
                           return NULL;
      );
    } PHP5TO7_ZEND_HASH_FOREACH_END(points);

  } PHP5TO7_ZEND_HASH_FOREACH_END(line_strings);

  ASSERT_SUCCESS_BLOCK(dse_polygon_finish(result),
                       dse_polygon_free(result);
                       return NULL;
  );

  return result;
}

int php_driver_polygon_bind_by_index(CassStatement *statement,
                                     size_t index,
                                     zval *value TSRMLS_DC)
{
  php_driver_polygon *polygon = PHP_DRIVER_GET_POLYGON(value);
  HashTable *line_strings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->rings);

  DsePolygon *dse_polygon = build_polygon(line_strings TSRMLS_CC);

  ASSERT_SUCCESS_BLOCK(cass_statement_bind_dse_polygon(statement,
                                                       index,
                                                       dse_polygon),
                       dse_polygon_free(dse_polygon);
      return FAILURE;
  );

  dse_polygon_free(dse_polygon);
  return SUCCESS;
}

int php_driver_polygon_bind_by_name(CassStatement *statement,
                                    const char *name,
                                    zval *value TSRMLS_DC)
{
  php_driver_polygon *polygon = PHP_DRIVER_GET_POLYGON(value);
  HashTable *line_strings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->rings);

  DsePolygon *dse_polygon = build_polygon(line_strings TSRMLS_CC);

  ASSERT_SUCCESS_BLOCK(cass_statement_bind_dse_polygon_by_name(statement,
                                                               name,
                                                               dse_polygon),
                       dse_polygon_free(dse_polygon);
      return FAILURE;
  );

  dse_polygon_free(dse_polygon);
  return SUCCESS;
}

int php_driver_polygon_construct_from_value(const CassValue *value,
                                            php5to7_zval *out TSRMLS_DC)
{
  int rc;
  DsePolygonIterator* iterator = dse_polygon_iterator_new();

  ASSERT_SUCCESS_BLOCK(dse_polygon_iterator_reset(iterator, value),
                       dse_polygon_iterator_free(iterator);
      return FAILURE;
  );

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), php_driver_polygon_ce);
  rc = php_driver_polygon_construct_from_iterator(iterator,
                                                  PHP5TO7_ZVAL_MAYBE_DEREF(out) TSRMLS_CC);
  dse_polygon_iterator_free(iterator);
  return rc;
}

char *php_driver_polygon_to_wkt(php_driver_polygon *polygon TSRMLS_DC)
{
  char *result = NULL;
  HashTable *rings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->rings);

  if (zend_hash_num_elements(rings) > 0) {
    // Traverse the rings array and iteratively build "wkt".
    php5to7_ulong num_key;
    php5to7_zval *ring_obj;
    smart_str wkt = PHP5TO7_SMART_STR_INIT;
    smart_str_appends(&wkt, "POLYGON (");

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(rings, num_key, ring_obj) {
      char *ring_wkt;
      php_driver_line_string *ring = PHP_DRIVER_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(ring_obj));

      if (num_key > 0) {
        // Add the comma that separates rings in the wkt.
        smart_str_appends(&wkt, ", ");
      }

      // Add the ring to the wkt. We use the wkt of the linestring, which is of the form
      //   LINESTRING (x1 y1, x2 y2, ...)
      // and extract the data part (e.g. starting at index 11, the open-paren).
      ring_wkt = php_driver_line_string_to_wkt(ring TSRMLS_CC);
      smart_str_appends(&wkt, ring_wkt + 11);
      efree(ring_wkt);
    } PHP5TO7_ZEND_HASH_FOREACH_END(rings);

    // Wrap up the wkt.
    smart_str_appendc(&wkt, ')');
    smart_str_0(&wkt);
    result = ecalloc(PHP5TO7_SMART_STR_LEN(wkt) + 1, 1);
    strncpy(result, PHP5TO7_SMART_STR_VAL(wkt), PHP5TO7_SMART_STR_LEN(wkt));
    smart_str_free(&wkt);
  } else {
    spprintf(&result, 0, "%s", "POLYGON EMPTY");
  }

  return result;
}

char *php_driver_polygon_to_string(php_driver_polygon *polygon TSRMLS_DC)
{
  HashTable *rings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->rings);
  char* result = NULL;

  if (zend_hash_num_elements(rings) > 0) {
    // Traverse the rings array and iteratively build the to-string rep.

    php5to7_ulong num_key;
    php5to7_zval *ring_obj;
    char *ring_string;
    smart_str to_string = PHP5TO7_SMART_STR_INIT;

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(rings, num_key, ring_obj) {
      php_driver_line_string *ring = PHP_DRIVER_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(ring_obj));

      // Ok, things get a little funky here. We want to write out the exterior ring separately from
      // interior rings. Concretely, we want output to look like this:
      //
      // Exterior ring: x1,y1 to x2,y2 to ...
      // Interior rings:
      //     x1,y1 to x2,y2 to ...
      //     x11,y11 to x12,y12 to ...
      //
      // We use the index (num_key) to separate cases.

      if (num_key == 0) {
        // We're emitting the exterior-ring, so add the title first.
        smart_str_appends(&to_string, "Exterior ring: ");
      } else {
        if (num_key == 1) {
          // This is our first interior-ring, so add its title.
          smart_str_appends(&to_string, "\nInterior rings:");
        }
        // Every interior ring is preceded by newline and indent.
        smart_str_appends(&to_string, "\n    ");
      }

      // Add the ring to to_string.
      ring_string = php_driver_line_string_to_string(ring TSRMLS_CC);
      smart_str_appends(&to_string, ring_string);
      efree(ring_string);
    } PHP5TO7_ZEND_HASH_FOREACH_END(rings);

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

int php_driver_polygon_construct_from_iterator(DsePolygonIterator* iterator,
                                            zval *return_value TSRMLS_DC)
{
  php_driver_polygon *polygon;
  size_t i, num_rings;
  HashTable *line_strings;

  polygon = PHP_DRIVER_GET_POLYGON(return_value);
  line_strings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->rings);
  num_rings = dse_polygon_iterator_num_rings(iterator);

  for (i = 0; i < num_rings; ++i) {
    php5to7_zval zline_string;
    php_driver_line_string *line_string;
    cass_uint32_t j, num_points;
    HashTable *points;

    PHP5TO7_ZVAL_MAYBE_MAKE(zline_string);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zline_string), php_driver_line_string_ce);
    line_string = PHP_DRIVER_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_P(zline_string));
    points = PHP5TO7_Z_ARRVAL_MAYBE_P(line_string->points);
    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(line_strings,
                                        PHP5TO7_ZVAL_MAYBE_P(zline_string),
                                        sizeof(zval *));

    ASSERT_SUCCESS_VALUE(dse_polygon_iterator_next_num_points(iterator, &num_points),
                         FAILURE);

    for (j = 0; j < num_points; ++j) {
      php5to7_zval zpoint;
      php_driver_point *point;

      PHP5TO7_ZVAL_MAYBE_MAKE(zpoint);
      object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zpoint), php_driver_point_ce);
      point = PHP_DRIVER_GET_POINT(PHP5TO7_ZVAL_MAYBE_P(zpoint));
      PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(points,
                                          PHP5TO7_ZVAL_MAYBE_P(zpoint),
                                          sizeof(zval *));

      ASSERT_SUCCESS_VALUE(dse_polygon_iterator_next_point(iterator, &point->x, &point->y),
                           FAILURE);
    }
  }

  return SUCCESS;
}

PHP_METHOD(Polygon, __construct)
{
  php5to7_zval_args args;
  php_driver_polygon *self = PHP_DRIVER_GET_POLYGON(getThis());
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  if (num_args == 1) {
    zval *ring_obj_or_wkt = PHP5TO7_ZVAL_ARG(args[0]);

    if (Z_TYPE_P(ring_obj_or_wkt) == IS_STRING) {
      int rc;

      // Create and initialize a "text iterator" which parses wkt. If there's a parse
      // error, it typically occurs during initialization. However, it's theoretically
      // possible to run into an issue when traversing the iterator, so cover that, too.

      DsePolygonIterator* iterator = dse_polygon_iterator_new();
      rc = dse_polygon_iterator_reset_with_wkt_n(iterator, Z_STRVAL_P(ring_obj_or_wkt), Z_STRLEN_P(ring_obj_or_wkt));
      if (rc == CASS_OK) {
        rc = php_driver_polygon_construct_from_iterator(iterator, getThis() TSRMLS_CC);
      }

      dse_polygon_iterator_free(iterator);

      if (rc != SUCCESS && rc != CASS_OK) {
        // Failed to parse the wkt properly. Yell.
        throw_invalid_argument(ring_obj_or_wkt, "Argument 1", "valid WKT for a Polygon" TSRMLS_CC);
      }

      PHP5TO7_MAYBE_EFREE(args);
      return;
    }
  }

  if (num_args > 0) {
    int i;

    // Every arg must be a LineString, and each LineString must have at least three points.
    for (i = 0; i < num_args; ++i) {
      php_driver_line_string *ring;
      HashTable *points;

      zval* ring_obj = PHP5TO7_ZVAL_ARG(args[i]);
      if (Z_TYPE_P(ring_obj) != IS_OBJECT || Z_OBJCE_P(ring_obj) != php_driver_line_string_ce) {
        char *object_name;
        spprintf(&object_name, 0, "Argument %d", i+1);
        throw_invalid_argument(ring_obj, object_name, "an instance of " PHP_DRIVER_NAMESPACE "\\LineString" TSRMLS_CC);
        efree(object_name);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }

      // Verify that the ring (linestring) has at least three points.
      ring = PHP_DRIVER_GET_LINE_STRING(ring_obj);
      points = PHP5TO7_Z_ARRVAL_MAYBE_P(ring->points);
      if (zend_hash_num_elements(points) < 3) {
        char *object_name;
        spprintf(&object_name, 0, "Argument %d", i+1);
        throw_invalid_argument(ring_obj, object_name, "a " PHP_DRIVER_NAMESPACE "\\LineString with at least three points" TSRMLS_CC);
        efree(object_name);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
    }

    for (i = 0; i < num_args; ++i) {
      zval* line_string = PHP5TO7_ZVAL_ARG(args[i]);
      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(self->rings), line_string);
      Z_TRY_ADDREF_P(line_string);
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

PHP_METHOD(Polygon, __toString)
{
  php_driver_polygon *self = NULL;
  char *to_string = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POLYGON(getThis());
  to_string = php_driver_polygon_to_string(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(to_string);
  efree(to_string);
}

PHP_METHOD(Polygon, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(PHP_DRIVER_G(type_polygon))) {
    PHP_DRIVER_G(type_polygon) = php_driver_type_custom(DSE_POLYGON_TYPE,
                                                        strlen(DSE_POLYGON_TYPE) TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(PHP_DRIVER_G(type_polygon)), 1, 0);
}

PHP_METHOD(Polygon, exteriorRing)
{
  php_driver_polygon *self = NULL;
  php5to7_zval *value;
  HashTable *rings;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POLYGON(getThis());

  rings = PHP5TO7_Z_ARRVAL_MAYBE_P(self->rings);

  if (PHP5TO7_ZEND_HASH_INDEX_FIND(rings, 0, value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(Polygon, rings)
{
  php_driver_polygon *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POLYGON(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->rings), 1, 0);
}

PHP_METHOD(Polygon, interiorRings)
{
  php_driver_polygon *self = NULL;
  php5to7_ulong num_key;
  php5to7_zval *ring_obj;
  HashTable *rings;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POLYGON(getThis());
  rings = PHP5TO7_Z_ARRVAL_MAYBE_P(self->rings);
  array_init(return_value);

  PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(rings, num_key, ring_obj) {
    // The first ring is the exterior ring, so we skip it.
    // NB: We can't write this with an 'num_key == 0' and 'continue' because this foreach
    // macro mechanism doesn't move the iterator forward in PHP5 if we do that.
    if (num_key > 0) {
      add_next_index_zval(return_value, PHP5TO7_ZVAL_MAYBE_DEREF(ring_obj));
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(ring_obj));
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(rings);
}

PHP_METHOD(Polygon, ring)
{
  ulong index;
  php_driver_polygon *self = NULL;
  php5to7_zval *value;
  HashTable *rings;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_POLYGON(getThis());

  rings = PHP5TO7_Z_ARRVAL_MAYBE_P(self->rings);

  if (PHP5TO7_ZEND_HASH_INDEX_FIND(rings, index, value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(Polygon, wkt)
{
  php_driver_polygon *self = NULL;
  char *wkt = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_POLYGON(getThis());
  wkt = php_driver_polygon_to_wkt(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(wkt);
  efree(wkt);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, rings)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ring, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_polygon_methods[] = {
  PHP_ME(Polygon, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Polygon, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, exteriorRing, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, rings, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, interiorRings, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, ring, arginfo_ring, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Polygon, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_polygon_handlers;

static HashTable *
php_driver_polygon_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  php_driver_polygon *self = PHP_DRIVER_GET_POLYGON(object);

  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->rings));
  PHP5TO7_ZEND_HASH_UPDATE(props, "rings", sizeof("rings"), PHP5TO7_ZVAL_MAYBE_P(self->rings), sizeof(zval));

  return props;
}

static int
php_driver_polygon_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_polygon *left;
  php_driver_polygon *right;
  HashTable *left_rings;
  HashTable *right_rings;
  unsigned left_num_rings;
  unsigned right_num_rings;
  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  left = PHP_DRIVER_GET_POLYGON(obj1);
  right = PHP_DRIVER_GET_POLYGON(obj2);

  // Compare ring's
  left_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(left->rings);
  right_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(right->rings);
  left_num_rings = zend_hash_num_elements(left_rings);
  right_num_rings = zend_hash_num_elements(right_rings);

  // The polygon with fewer rings is the "lesser" of the two.
  if (left_num_rings != right_num_rings) {
    return left_num_rings < right_num_rings ? -1 : 1;
  }

  zend_hash_internal_pointer_reset_ex(left_rings, &pos1);
  zend_hash_internal_pointer_reset_ex(right_rings, &pos2);

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(left_rings, current1, &pos1) &&
         PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(right_rings, current2, &pos2)) {
    int result = php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                         PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (result != 0) return result;

    zend_hash_move_forward_ex(left_rings, &pos1);
    zend_hash_move_forward_ex(right_rings, &pos2);
  }

  return 0;
}

static void
php_driver_polygon_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_polygon *self = PHP5TO7_ZEND_OBJECT_GET(polygon, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->rings);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_polygon_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_polygon *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(polygon, ce);

  PHP5TO7_ZVAL_MAYBE_MAKE(self->rings);
  array_init(PHP5TO7_ZVAL_MAYBE_P(self->rings));

  PHP5TO7_ZEND_OBJECT_INIT(polygon, self, ce);
}

void php_driver_define_Polygon(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Polygon", php_driver_polygon_methods);
  php_driver_polygon_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_custom_ce);
  php_driver_polygon_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_polygon_ce->create_object = php_driver_polygon_new;

  memcpy(&php_driver_polygon_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_polygon_handlers.get_properties  = php_driver_polygon_properties;
  php_driver_polygon_handlers.compare_objects = php_driver_polygon_compare;
  php_driver_polygon_handlers.clone_obj = NULL;
}
