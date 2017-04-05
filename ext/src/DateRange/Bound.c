/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"

#include "util/params.h"
#include "util/types.h"

#include <ext/date/php_date.h>

zend_class_entry *php_driver_date_range_bound_ce = NULL;

int php_driver_validate_bound_precision(long precision) {
  return (precision != DSE_DATE_RANGE_PRECISION_UNBOUNDED &&
      (precision < DSE_DATE_RANGE_PRECISION_YEAR || precision > DSE_DATE_RANGE_PRECISION_MILLISECOND)) ?
         FAILURE : SUCCESS;
}

int php_driver_bound_get_time_from_zval(zval* time_ms_zval, const char* param_name, cass_int64_t* time_ms TSRMLS_DC)
{
  // Pull out the time_ms long from the arg; or if it's a DateTime, pull it from there.
  if (Z_TYPE_P(time_ms_zval) == IS_OBJECT && Z_OBJCE_P(time_ms_zval) == php_date_get_date_ce()) {
    php5to7_zval retval;

    zend_call_method_with_0_params(PHP5TO7_ZVAL_MAYBE_ADDR_OF(time_ms_zval),
      php_date_get_date_ce(),
      NULL,
      "gettimestamp",
      &retval);

    if (!PHP5TO7_ZVAL_IS_UNDEF(retval) && Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(retval)) == IS_LONG) {
      *time_ms = PHP5TO7_Z_LVAL_MAYBE_P(retval) * 1000;
      zval_ptr_dtor(&retval);
    } else {
      return FAILURE;
    }
  } else if (php_driver_get_int64_param(time_ms_zval, param_name, INT64_MIN, INT64_MAX, "a DateTime, ", time_ms TSRMLS_CC) == FAILURE) {
    return FAILURE;
  }
  return SUCCESS;
}

PHP_METHOD(Bound, __construct)
{
  long precision;
  cass_int64_t time_ms;
  zval* time_ms_zval;
  php_driver_date_range_bound *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &precision, &time_ms_zval) == FAILURE) {
    return;
  }

  if (php_driver_bound_get_time_from_zval(time_ms_zval, "timeMs", &time_ms TSRMLS_CC) == FAILURE) {
    return;
  }

  if (php_driver_validate_bound_precision(precision) == FAILURE) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "precision must be a value from the Precision class, %d given", precision);
    return;
  }

  self = PHP_DRIVER_GET_DATE_RANGE_BOUND(getThis());
  self->precision = precision;
  self->time_ms = time_ms;
}

PHP_METHOD(Bound, precision)
{
  php_driver_date_range_bound *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE_BOUND(getThis());
  RETURN_LONG(self->precision);
}

PHP_METHOD(Bound, timeMs)
{
  php_driver_date_range_bound *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE_BOUND(getThis());
  php_driver_int64_to_string(return_value, self->time_ms);
}

PHP_METHOD(Bound, unbounded)
{
  if (PHP5TO7_ZVAL_IS_UNDEF(PHP_DRIVER_G(unbounded_bound))) {
    php_driver_date_range_bound *bound;
    php5to7_zval bound_zval;

    PHP5TO7_ZVAL_MAYBE_MAKE(bound_zval);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(bound_zval), php_driver_date_range_bound_ce);
    bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(bound_zval));

    bound->time_ms = -1;
    bound->precision = DSE_DATE_RANGE_PRECISION_UNBOUNDED;

    PHP_DRIVER_G(unbounded_bound) = bound_zval;
  }
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(PHP_DRIVER_G(unbounded_bound)), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, precision)
  ZEND_ARG_INFO(0, timeMs)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_bound_methods[] = {
  PHP_ME(Bound, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Bound, precision, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bound, timeMs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bound, unbounded, arginfo_none, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_bound_handlers;

static HashTable *
php_driver_bound_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  php_driver_date_range_bound *self = PHP_DRIVER_GET_DATE_RANGE_BOUND(object);

  php5to7_zval precision, time_ms;
  PHP5TO7_ZVAL_MAYBE_MAKE(precision);
  PHP5TO7_ZVAL_MAYBE_MAKE(time_ms);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(precision), self->precision);
  php_driver_int64_to_string(PHP5TO7_ZVAL_MAYBE_P(time_ms), self->time_ms);
  PHP5TO7_ZEND_HASH_UPDATE(props, "precision", sizeof("precision"), PHP5TO7_ZVAL_MAYBE_P(precision), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "timeMs", sizeof("timeMs"), PHP5TO7_ZVAL_MAYBE_P(time_ms), sizeof(zval));

  return props;
}

static int
php_driver_bound_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_date_range_bound *left;
  php_driver_date_range_bound *right;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  left = PHP_DRIVER_GET_DATE_RANGE_BOUND(obj1);
  right = PHP_DRIVER_GET_DATE_RANGE_BOUND(obj2);

  // Special case: bounds that have an UNBOUNDED precision are
  // equal, regardless of time_ms.
  if (left->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED && left->precision == right->precision) {
    return 0;
  }

  // Comparisons compare time_ms, then precision

  if (left->time_ms != right->time_ms) {
    return left->time_ms < right->time_ms ? -1 : 1;
  }
  if (left->precision != right->precision) {
    return left->precision < right->precision ? -1 : 1;
  }
  return 0;
}

static void
php_driver_bound_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_date_range_bound *self = PHP5TO7_ZEND_OBJECT_GET(date_range_bound, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_bound_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_date_range_bound *self = PHP5TO7_ZEND_OBJECT_ECALLOC(date_range_bound, ce);
  PHP5TO7_ZEND_OBJECT_INIT_EX(date_range_bound, bound, self, ce);
}

void php_driver_define_DateRangeBound(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DateRange\\Bound", php_driver_bound_methods);
  php_driver_date_range_bound_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_date_range_bound_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_date_range_bound_ce->create_object = php_driver_bound_new;

  memcpy(&php_driver_bound_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_bound_handlers.get_properties  = php_driver_bound_properties;
  php_driver_bound_handlers.compare_objects = php_driver_bound_compare;
  php_driver_bound_handlers.clone_obj = NULL;
}
