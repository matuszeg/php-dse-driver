#include "php_dse.h"
#include "php_dse_globals.h"
#include "php_dse_types.h"
#include "LineString.h"

// For php_cassandra_value_compare
#include "util/hash.h"

#include "util/types.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *dse_polygon_ce = NULL;

#define DSE_POLYGON_TYPE "org.apache.cassandra.db.marshal.PolygonType"
char *polygon_to_wkt(dse_polygon *polygon TSRMLS_DC)
{
  char *result = NULL;
  dse_line_string *exterior_ring = NULL;
  HashTable *interior_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->interior_rings);

  if (zend_hash_num_elements(interior_rings) > 0 || !PHP5TO7_ZVAL_IS_UNDEF_P(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring))) {
    // Traverse the interior_rings array and iteratively build "wkt".
    exterior_ring = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring));
    php5to7_ulong num_key;
    php5to7_zval *interior_ring_obj;
    smart_str wkt = PHP5TO7_SMART_STR_INIT;
    smart_str_appends(&wkt, "POLYGON (");

    // Add the exterior ring to the wkt. We use the wkt of the linestring, which is of the form
    //   LINESTRING (x1 y1, x2 y2, ...)
    // and extract the data part (e.g. starting at index 11, the open-paren).
    char *ring_wkt = line_string_to_wkt(exterior_ring TSRMLS_CC);
    smart_str_appends(&wkt, ring_wkt + 11);
    efree(ring_wkt);

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(interior_rings, num_key, interior_ring_obj) {
      dse_line_string *ring = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(interior_ring_obj));

      // Add the comma that separates interior_rings in the wkt.
      smart_str_appends(&wkt, ", ");

      // Add the next ring to the wkt. Again, we use the wkt of the linestring, which is of the form
      //   LINESTRING (x1 y1, x2 y2, ...)
      // and extract the data part (e.g. starting at index 11, the open-paren).
      ring_wkt = line_string_to_wkt(ring TSRMLS_CC);
      smart_str_appends(&wkt, ring_wkt + 11);
      efree(ring_wkt);
    } PHP5TO7_ZEND_HASH_FOREACH_END(interior_rings);

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

char *polygon_to_string(dse_polygon *polygon TSRMLS_DC)
{
  dse_line_string *exterior_ring = NULL;
  HashTable *interior_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(polygon->interior_rings);
  char* result = NULL;

  if (zend_hash_num_elements(interior_rings) > 0 || !PHP5TO7_ZVAL_IS_UNDEF_P(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring))) {
    // Traverse the interior_rings array and iteratively build the to-string rep.
    exterior_ring = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring));

    php5to7_ulong num_key;
    php5to7_zval *interior_ring_obj;
    char *ring_string;
    smart_str to_string = PHP5TO7_SMART_STR_INIT;
    smart_str_appends(&to_string, "Exterior ring: ");
    ring_string = line_string_to_string(exterior_ring TSRMLS_CC);
    smart_str_appends(&to_string, ring_string);
    efree(ring_string);
    smart_str_appends(&to_string, "\nInterior rings:");

    PHP5TO7_ZEND_HASH_FOREACH_NUM_KEY_VAL(interior_rings, num_key, interior_ring_obj) {
      dse_line_string *interior_ring = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(interior_ring_obj));

      // Add the newline and indent that separates line-strings in toString.
      smart_str_appends(&to_string, "\n    ");

      // Add the next interior_ring to to_string.
      ring_string = line_string_to_string(interior_ring TSRMLS_CC);
      smart_str_appends(&to_string, ring_string);
      efree(ring_string);
    } PHP5TO7_ZEND_HASH_FOREACH_END(interior_rings);

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

static DsePolygon *
build_polygon(HashTable *line_strings TSRMLS_DC)
{
  php5to7_zval *current1;
  DsePolygon *result = dse_polygon_new();

  PHP5TO7_ZEND_HASH_FOREACH_VAL(line_strings, current1) {
    php5to7_zval *current2;
    dse_line_string *line_string = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_DEREF(current1));
    HashTable *points = NULL; /* TODO: Set this */

    ASSERT_SUCCESS_BLOCK(dse_polygon_start_ring(result),
                         dse_polygon_free(result);
                         return NULL;
    );

    PHP5TO7_ZEND_HASH_FOREACH_VAL(points, current2) {
      dse_point *point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_DEREF(current2));
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

static int
marshal_bind_by_index(CassStatement *statement, size_t index, zval *value TSRMLS_DC)
{
  dse_polygon *polygon = PHP_DSE_GET_POLYGON(value);
  HashTable *line_strings = NULL; /* TODO: Set this */

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

static int
marshal_bind_by_name(CassStatement *statement, const char *name, zval *value TSRMLS_DC)
{
  dse_polygon *polygon = PHP_DSE_GET_POLYGON(value);
  HashTable *line_strings = NULL; /* TODO: Set this */

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

static int
marshal_get_result(const CassValue *value, php5to7_zval *out TSRMLS_DC)
{
  dse_polygon *polygon;
  size_t i, num_rings;
  HashTable *line_strings = NULL; /* TODO: Set this */
  DsePolygonIterator* iterator = dse_polygon_iterator_new();

  ASSERT_SUCCESS_BLOCK(dse_polygon_iterator_reset(iterator, value),
                       dse_polygon_iterator_free(iterator);
                       return FAILURE;
  );

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), dse_polygon_ce);
  polygon = PHP_DSE_GET_POLYGON(PHP5TO7_ZVAL_MAYBE_DEREF(out));

  num_rings = dse_polygon_iterator_num_rings(iterator);

  for (i = 0; i < num_rings; ++i) {
    php5to7_zval zline_string;
    dse_line_string *line_string;
    cass_uint32_t j, num_points;
    HashTable *points = NULL; /* TODO: Set this */

    PHP5TO7_ZVAL_MAYBE_MAKE(zline_string);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zline_string), dse_line_string_ce);
    line_string = PHP_DSE_GET_LINE_STRING(PHP5TO7_ZVAL_MAYBE_P(zline_string));
    PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(line_strings,
                                        PHP5TO7_ZVAL_MAYBE_P(zline_string),
                                        sizeof(zval *));

    ASSERT_SUCCESS_BLOCK(dse_polygon_iterator_next_num_points(iterator, &num_points),
                         dse_polygon_iterator_free(iterator);
                         return FAILURE;
    );

    for (j = 0; j < num_points; ++j) {
      php5to7_zval zpoint;
      dse_point *point;

      PHP5TO7_ZVAL_MAYBE_MAKE(zpoint);
      object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zpoint), dse_point_ce);
      point = PHP_DSE_GET_POINT(PHP5TO7_ZVAL_MAYBE_P(zpoint));
      PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(points,
                                          PHP5TO7_ZVAL_MAYBE_P(zpoint),
                                          sizeof(zval *));

      ASSERT_SUCCESS_BLOCK(dse_polygon_iterator_next_point(iterator, &point->x, &point->y),
                           dse_polygon_iterator_free(iterator);
                           return FAILURE;
      );
    }
  }

  return SUCCESS;
}

PHP_METHOD(DsePolygon, __construct)
{
  php5to7_zval_args args;
  dse_polygon *self = PHP_DSE_GET_POLYGON(getThis());
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  // The first param is the exterior-ring, if specified.
  if (num_args > 0) {
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring), PHP5TO7_ZVAL_ARG(args[0]));

    // TODO: Error out if any line-string's are empty.

    // The rest are interior-rings, if specified.
    for (int i = 1; i < num_args; ++i) {
      zval* line_string = PHP5TO7_ZVAL_ARG(args[i]);
      add_next_index_zval(PHP5TO7_ZVAL_MAYBE_P(self->interior_rings), line_string);
      Z_TRY_ADDREF_P(line_string);
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

PHP_METHOD(DsePolygon, __toString)
{
  dse_polygon *self = NULL;
  char *to_string = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POLYGON(getThis());
  to_string = polygon_to_string(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(to_string);
  efree(to_string);
}

PHP_METHOD(DsePolygon, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(DSE_G(type_polygon))) {
    DSE_G(type_polygon) = php_cassandra_type_custom(DSE_POLYGON_TYPE TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(DSE_G(type_polygon)), 1, 0);
}

PHP_METHOD(DsePolygon, exteriorRing)
{
  dse_polygon *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POLYGON(getThis());

  if (!PHP5TO7_ZVAL_IS_UNDEF_P(PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring))) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring), 1, 0);
  }
}

PHP_METHOD(DsePolygon, interiorRings)
{
  dse_polygon *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POLYGON(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->interior_rings), 1, 0);
}

PHP_METHOD(DsePolygon, interiorRing)
{
  ulong index;
  dse_polygon *self = NULL;
  php5to7_zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_POLYGON(getThis());

  HashTable *rings = PHP5TO7_Z_ARRVAL_MAYBE_P(self->interior_rings);

  if (PHP5TO7_ZEND_HASH_INDEX_FIND(rings, index, value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(*value), 1, 0);
  }
}

PHP_METHOD(DsePolygon, wkt)
{
  dse_polygon *self = NULL;
  char *wkt = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POLYGON(getThis());
  wkt = polygon_to_wkt(self TSRMLS_CC);
  PHP5TO7_RETVAL_STRING(wkt);
  efree(wkt);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, rings)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_interiorRing, 0, ZEND_RETURN_VALUE, 0)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static zend_function_entry dse_polygon_methods[] = {
  PHP_ME(DsePolygon, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DsePolygon, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, exteriorRing, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, interiorRings, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, interiorRing, arginfo_interiorRing, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_polygon_handlers;

static HashTable *
php_dse_polygon_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  dse_polygon *self = PHP_DSE_GET_POLYGON(object);

  if (!PHP5TO7_ZVAL_IS_UNDEF_P(PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring))) {
    Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring));
    PHP5TO7_ZEND_HASH_UPDATE(props, "exteriorRing", sizeof("exteriorRing"),
      PHP5TO7_ZVAL_MAYBE_P(self->exterior_ring), sizeof(zval));
  }
  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->interior_rings));
  PHP5TO7_ZEND_HASH_UPDATE(props, "interiorRings", sizeof("interiorRings"),
    PHP5TO7_ZVAL_MAYBE_P(self->interior_rings), sizeof(zval));

  return props;
}

static int
php_dse_polygon_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  dse_polygon *left = PHP_DSE_GET_POLYGON(obj1);
  dse_polygon *right = PHP_DSE_GET_POLYGON(obj2);

  // First compare exterior-ring's
  int result = php_cassandra_value_compare(PHP5TO7_ZVAL_MAYBE_P(left->exterior_ring),
                                           PHP5TO7_ZVAL_MAYBE_P(right->exterior_ring) TSRMLS_CC);

  if (result != 0) return result;

  // Now compare interior-ring's
  HashTable *left_interior_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(left->interior_rings);
  HashTable *right_interior_rings = PHP5TO7_Z_ARRVAL_MAYBE_P(right->interior_rings);
  unsigned left_num_interior_rings = zend_hash_num_elements(left_interior_rings);
  unsigned right_num_interior_rings = zend_hash_num_elements(right_interior_rings);

  // The polygon with fewer interior rings is the "lesser" of the two.
  if (left_num_interior_rings != right_num_interior_rings) {
    return left_num_interior_rings < right_num_interior_rings ? -1 : 1;
  }

  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;

  zend_hash_internal_pointer_reset_ex(left_interior_rings, &pos1);
  zend_hash_internal_pointer_reset_ex(right_interior_rings, &pos2);

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(left_interior_rings, current1, &pos1) &&
         PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(right_interior_rings, current2, &pos2)) {
    result = php_cassandra_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                         PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (result != 0) return result;

    zend_hash_move_forward_ex(left_interior_rings, &pos1);
    zend_hash_move_forward_ex(right_interior_rings, &pos2);
  }

  return 0;
}

static void
php_dse_polygon_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_polygon *self = PHP5TO7_ZEND_OBJECT_GET(dse_polygon, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->exterior_ring);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->interior_rings);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_polygon_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_polygon *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_polygon, ce);

  PHP5TO7_ZVAL_UNDEF(self->exterior_ring);

  PHP5TO7_ZVAL_MAYBE_MAKE(self->interior_rings);
  array_init(PHP5TO7_ZVAL_MAYBE_P(self->interior_rings));

  PHP5TO7_ZEND_OBJECT_INIT(dse_polygon, self, ce);
}

void dse_define_Polygon(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Polygon", dse_polygon_methods);
  dse_polygon_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_polygon_ce TSRMLS_CC, 1, cassandra_custom_ce);
  dse_polygon_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_polygon_ce->create_object = php_dse_polygon_new;

  memcpy(&dse_polygon_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_polygon_handlers.get_properties  = php_dse_polygon_properties;
  dse_polygon_handlers.compare_objects = php_dse_polygon_compare;
  dse_polygon_handlers.clone_obj = NULL;

  php_cassandra_custom_marshal_add(DSE_POLYGON_TYPE,
                                   marshal_bind_by_index,
                                   marshal_bind_by_name,
                                   marshal_get_result TSRMLS_CC);
}
