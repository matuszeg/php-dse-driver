#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_polygon_ce = NULL;


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
  dse_polygon_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_polygon_ce->create_object = php_dse_polygon_new;

  memcpy(&dse_polygon_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_polygon_handlers.get_properties  = php_dse_polygon_properties;
  dse_polygon_handlers.compare_objects = php_dse_polygon_compare;
  dse_polygon_handlers.clone_obj = NULL;
}
