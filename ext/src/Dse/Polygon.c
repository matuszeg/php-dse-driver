#include "php_dse.h"
#include "php_dse_globals.h"
#include "php_dse_types.h"

#include "util/types.h"

zend_class_entry *dse_polygon_ce = NULL;

#define DSE_POLYGON_TYPE "org.apache.cassandra.db.marshal.PolygonType"

// Helper function to copy the points collection out of a line-string struct
// to a previously initialized array.
static void
copy_interior(dse_polygon *polygon, zval *array)
{
  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(&polygon->interior_rings, current) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_DEREF(current)) == SUCCESS)
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    else
      break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(&polygon->interior_rings);
}

static int
to_string(zval *result, dse_polygon *polygon TSRMLS_DC)
{
  char *string;
  spprintf(&string, 0, "__NOT_IMPLEMENTED__");
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
  return SUCCESS;
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
  dse_polygon *polygon = PHP_DSE_GET_POLYGON(getThis());
  int num_args = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  // The first param is the exterior-ring, if specified.
  if (num_args > 0) {
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring), PHP5TO7_ZVAL_ARG(args[0]));

    // The rest are interior-rings, if specified.
    for (int i = 1; i < num_args; ++i) {
      zval* line_string = PHP5TO7_ZVAL_ARG(args[i]);
      if (PHP5TO7_ZEND_HASH_NEXT_INDEX_INSERT(&polygon->interior_rings, line_string, sizeof(zval *))) {
        Z_TRY_ADDREF_P(line_string);
      }
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

PHP_METHOD(DsePolygon, __toString)
{
  dse_polygon *self = PHP_DSE_GET_POLYGON(getThis());

  to_string(return_value, self TSRMLS_CC);
}

PHP_METHOD(DsePolygon, type)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(DSE_G(type_polygon))) {
    DSE_G(type_polygon) = php_cassandra_type_custom(DSE_POLYGON_TYPE TSRMLS_CC);
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(DSE_G(type_polygon)), 1, 0);
}

PHP_METHOD(DsePolygon, exterior_ring)
{
  dse_polygon *polygon = PHP_DSE_GET_POLYGON(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(polygon->exterior_ring), 1, 0);
}

PHP_METHOD(DsePolygon, interior_rings)
{
  dse_polygon *polygon = NULL;
  array_init(return_value);
  polygon = PHP_DSE_GET_POLYGON(getThis());
  copy_interior(polygon, return_value);
}

PHP_METHOD(DsePolygon, wkt)
{
  dse_polygon *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DSE_GET_POLYGON(getThis());
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
ZEND_ARG_INFO(0, rings)
ZEND_END_ARG_INFO()

static zend_function_entry dse_polygon_methods[] = {
  PHP_ME(DsePolygon, __construct,  arginfo__construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DsePolygon, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, exterior_ring, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, interior_rings, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, wkt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DsePolygon, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_polygon_handlers;

static HashTable *
php_dse_polygon_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_polygon_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_polygon_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_polygon *self = PHP5TO7_ZEND_OBJECT_GET(dse_polygon, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_polygon_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_polygon *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_polygon, ce);

  PHP5TO7_ZVAL_UNDEF(self->exterior_ring);
  PHP5TO7_ZVAL_MAYBE_MAKE(self->exterior_ring);

  zend_hash_init(&self->interior_rings, 0, NULL, ZVAL_PTR_DTOR, 0);

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
