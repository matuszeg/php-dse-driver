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

#include "DateRange.h"
#include "DateRange/Bound.h"

zend_class_entry *php_driver_date_range_ce = NULL;

enum ArgParseState {
  BEGIN,
  NEED_TIME
};
/************ Define some DSE graph functions here temporarily ****** */
CassError dse_graph_array_add_date_range(DseGraphArray* array,
                                         const DseDateRange* value) {
  return CASS_ERROR_LIB_NOT_IMPLEMENTED;
/*
  if (array->is_complete()) {
    return CASS_ERROR_LIB_BAD_PARAMS;
  }
  array->add_date_range(value->from());
  return CASS_OK;
*/
}

CassError dse_graph_object_add_date_range_n(DseGraphObject* object,
                                            const char* name,
                                            size_t name_length,
                                            const DseDateRange* value) {
  return CASS_ERROR_LIB_NOT_IMPLEMENTED;
/*
  if (object->is_complete()) {
    return CASS_ERROR_LIB_BAD_PARAMS;
  }
  object->add_key(name, name_length);
  object->add_date_range(value->from());
  return CASS_OK;
*/
}

CassError dse_graph_object_add_date_range(DseGraphObject* object,
                                          const char* name,
                                          const DseDateRange* value) {
  return dse_graph_object_add_date_range_n(object,
                                           name, strlen(name),
                                           value);
}
/************ Finish defining DSE graph functions ******* */

static DseDateRange*
build_dse_date_range(const php_driver_date_range* date_range TSRMLS_DC) {
  DseDateRange *dse_date_range = &PHP_DRIVER_G(dse_date_range);
  if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(date_range->upper_bound)) == IS_NULL) {
    php_driver_date_range_bound *bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(date_range->lower_bound));
    dse_date_range_init_single_date(dse_date_range,
                                    dse_date_range_bound_init(bound->precision, bound->time_ms));
  } else {
    php_driver_date_range_bound *lower_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(date_range->lower_bound));
    php_driver_date_range_bound *upper_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(date_range->upper_bound));
    dse_date_range_init(dse_date_range,
                        dse_date_range_bound_init(lower_bound->precision, lower_bound->time_ms),
                        dse_date_range_bound_init(upper_bound->precision, upper_bound->time_ms));
  }
  return dse_date_range;
}

static void precision_to_string(long precision, char* precision_string) {
  switch (precision) {
    case DSE_DATE_RANGE_PRECISION_YEAR:
      strcpy(precision_string, "YEAR");
      break;
    case DSE_DATE_RANGE_PRECISION_MONTH:
      strcpy(precision_string, "MONTH");
      break;
    case DSE_DATE_RANGE_PRECISION_DAY:
      strcpy(precision_string, "DAY");
      break;
    case DSE_DATE_RANGE_PRECISION_HOUR:
      strcpy(precision_string, "HOUR");
      break;
    case DSE_DATE_RANGE_PRECISION_MINUTE:
      strcpy(precision_string, "MINUTE");
      break;
    case DSE_DATE_RANGE_PRECISION_SECOND:
      strcpy(precision_string, "SECOND");
      break;
    case DSE_DATE_RANGE_PRECISION_MILLISECOND:
      strcpy(precision_string, "MILLISECOND");
      break;
    default:
      strcpy(precision_string, "UNKNOWN");
  }
}

static void time_to_string(cass_int64_t time_int, char* time_string) {
  /* time_int is ms-precision. */
  time_t time_secs = (time_t) time_int / 1000;
  strftime(time_string, 20, "%Y-%m-%d %H:%M:%S", gmtime(&time_secs));
  sprintf(time_string + 19, ".%03d", (int) (time_int % 1000));
}

#define EXPAND_PARAMS(date_range) build_dse_date_range(date_range TSRMLS_CC)
PHP_DRIVER_DEFINE_DSE_TYPE_HELPERS(date_range, DATE_RANGE, EXPAND_PARAMS)
#undef EXPAND_PARAMS

int php_driver_date_range_construct_from_value(const CassValue *value,
                                               php5to7_zval *out TSRMLS_DC)
{
  DseDateRange dse_date_range;
  php_driver_date_range *date_range;
  php_driver_date_range_bound *lower_bound, *upper_bound;

  object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(out), php_driver_date_range_ce);
  date_range = PHP_DRIVER_GET_DATE_RANGE(PHP5TO7_ZVAL_MAYBE_DEREF(out));

  ASSERT_SUCCESS_VALUE(cass_value_get_dse_date_range(value, &dse_date_range),
                       FAILURE);

  // Fill in date_range from dse_date_range
  PHP5TO7_ZVAL_MAYBE_MAKE(date_range->lower_bound);
  PHP5TO7_ZVAL_MAYBE_MAKE(date_range->upper_bound);

  // Default the upper bound to null.
  ZVAL_NULL(PHP5TO7_ZVAL_MAYBE_P(date_range->upper_bound));

  object_init_ex(PHP5TO7_ZVAL_MAYBE_P(date_range->lower_bound), php_driver_date_range_bound_ce);
  lower_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(date_range->lower_bound));
  lower_bound->precision = dse_date_range.lower_bound.precision;
  lower_bound->time_ms = dse_date_range.lower_bound.time_ms;

  if (!dse_date_range.is_single_date) {
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(date_range->upper_bound), php_driver_date_range_bound_ce);
    upper_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(date_range->upper_bound));
    upper_bound->precision = dse_date_range.upper_bound.precision;
    upper_bound->time_ms = dse_date_range.upper_bound.time_ms;
  }

  return SUCCESS;
}

PHP_METHOD(DateRange, __construct)
{
  php5to7_zval_args args;
  php_driver_date_range *self = NULL;
  int arg_index = 0, num_args = 0, bound_index = 0;
  php5to7_zval* bounds[2];
  int state = BEGIN;
  php_driver_date_range_bound *bound = NULL;
  char arg_name[20];

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*", &args, &num_args) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_DATE_RANGE(getThis());

  bounds[0] = &self->lower_bound;
  bounds[1] = &self->upper_bound;

  for ( ; arg_index < num_args && bound_index < 2; ++arg_index) {
    zval *arg = PHP5TO7_ZVAL_ARG(args[arg_index]);

    switch (state) {
    case BEGIN:
      if (Z_TYPE_P(arg) == IS_LONG) {
        // This arg is a precision. Make sure it's legal.
        PHP5TO7_ZVAL_MAYBE_MAKE(PHP5TO7_ZVAL_MAYBE_DEREF(bounds[bound_index]));
        object_init_ex(PHP5TO7_ZVAL_MAYBE_DEREF(bounds[bound_index]), php_driver_date_range_bound_ce);
        bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_DEREF(bounds[bound_index]));

        bound->precision = Z_LVAL_P(arg);
        if (php_driver_validate_bound_precision(bound->precision) == FAILURE) {
          sprintf(arg_name, "Argument %d", arg_index + 1);
          throw_invalid_argument(arg, arg_name, "a value from the Precision class" TSRMLS_CC);
          PHP5TO7_MAYBE_EFREE(args);
          return;
        }
        state = NEED_TIME;
      } else if (Z_TYPE_P(arg) == IS_OBJECT && Z_OBJCE_P(arg) == php_driver_date_range_bound_ce) {
        PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(*bounds[bound_index]), arg);
        bound_index++;
      } else {
        sprintf(arg_name, "Argument %d", arg_index + 1);
        throw_invalid_argument(arg, arg_name, "a value from the Precision class or a Bound object" TSRMLS_CC);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
      break;
    case NEED_TIME:
      // This arg is a time_ms. Make sure it's legal.
      sprintf(arg_name, "Argument %d", arg_index + 1);
      if (php_driver_bound_get_time_from_zval(arg, arg_name, &bound->time_ms TSRMLS_CC) == FAILURE) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
      state = BEGIN;
      bound_index++;
      break;
    default:
      // This should never happen
      zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
        "An internal error occurred when processing parameters to DateRange");
      PHP5TO7_MAYBE_EFREE(args);
      return;
    }
  }

  if (arg_index < num_args || state != BEGIN || num_args == 0) {
    zend_throw_exception_ex(spl_ce_BadFunctionCallException, 0 TSRMLS_CC,
                            "A DateRange may only be constructed with a lower and upper bound in the form of "
                            "Bound objects or <precision, time_ms|DateTime> pairs");
    if (num_args > 0) {
      PHP5TO7_MAYBE_EFREE(args);
    }
    return;
  }

  if (bound_index == 1) {
    // We don't have an upper bound, so initialize it to null.
    PHP5TO7_ZVAL_MAYBE_MAKE(self->upper_bound);
    ZVAL_NULL(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound));
  }

  PHP5TO7_MAYBE_EFREE(args);
}

PHP_METHOD(DateRange, __toString)
{
  char* rep;
  php_driver_date_range *self = NULL;
  char from_precision_string[20], to_precision_string[20], from_time_string[30], to_time_string[30];
  php_driver_date_range_bound *upper_bound, *lower_bound;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE(getThis());

  // Build up string representation of this date_range.
  lower_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(self->lower_bound));

  if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound)) == IS_NULL) {
    if (lower_bound->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED) {
      spprintf(&rep, 0, "*");
    } else {
      precision_to_string(lower_bound->precision, from_precision_string);
      time_to_string(lower_bound->time_ms, from_time_string);
      spprintf(&rep, 0, "%s(%s)", from_time_string, from_precision_string);
    }
  } else {
    upper_bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound));

    if (lower_bound->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED &&
      upper_bound->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED) {
      spprintf(&rep, 0, "* TO *");
    } else if (upper_bound->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED) {
      precision_to_string(lower_bound->precision, from_precision_string);
      time_to_string(lower_bound->time_ms, from_time_string);
      spprintf(&rep, 0, "%s(%s) TO *", from_time_string, from_precision_string);
    } else if (lower_bound->precision == DSE_DATE_RANGE_PRECISION_UNBOUNDED) {
      precision_to_string(upper_bound->precision, to_precision_string);
      time_to_string(upper_bound->time_ms, to_time_string);
      spprintf(&rep, 0, "* TO %s(%s)", to_time_string, to_precision_string);
    } else {
      precision_to_string(lower_bound->precision, from_precision_string);
      time_to_string(lower_bound->time_ms, from_time_string);
      precision_to_string(upper_bound->precision, to_precision_string);
      time_to_string(upper_bound->time_ms, to_time_string);
      spprintf(&rep, 0, "%s(%s) TO %s(%s)", from_time_string, from_precision_string, to_time_string, to_precision_string);
    }
  }

  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

PHP_METHOD(DateRange, type)
{
  php_driver_date_range_type(return_value TSRMLS_CC);
}

PHP_METHOD(DateRange, lowerBound)
{
  php_driver_date_range *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->lower_bound), 1, 0);
}

PHP_METHOD(DateRange, upperBound)
{
  php_driver_date_range *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound), 1, 0);
}

PHP_METHOD(DateRange, isSingleDate)
{
  php_driver_date_range *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DATE_RANGE(getThis());
  RETURN_BOOL(Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound)) == IS_NULL);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, bounds)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_date_range_methods[] = {
  PHP_ME(DateRange, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(DateRange, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DateRange, lowerBound, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DateRange, upperBound, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DateRange, isSingleDate, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DateRange, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_date_range_handlers;

static HashTable *
php_driver_date_range_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
  php_driver_date_range  *self = PHP_DRIVER_GET_DATE_RANGE(object);

  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->lower_bound));
  Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound));

  PHP5TO7_ZEND_HASH_UPDATE(props, "lowerBound", sizeof("lowerBound"), PHP5TO7_ZVAL_MAYBE_P(self->lower_bound), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "upperBound", sizeof("upperBound"), PHP5TO7_ZVAL_MAYBE_P(self->upper_bound), sizeof(zval));

  return props;
}

static int
php_driver_date_range_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_date_range *left;
  php_driver_date_range *right;
  int result, left_is_single, right_is_single;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  left = PHP_DRIVER_GET_DATE_RANGE(obj1);
  right = PHP_DRIVER_GET_DATE_RANGE(obj2);

  result = php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_P(left->lower_bound),
                                    PHP5TO7_ZVAL_MAYBE_P(right->lower_bound) TSRMLS_CC);
  if (result != 0) return result;

  left_is_single = Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(left->upper_bound)) == IS_NULL;
  right_is_single = Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(right->upper_bound)) == IS_NULL;

  // Compare upper bounds, if applicable.
  if (left_is_single) {
    return right_is_single ? 0 : -1;
  }

  // At this point, left is not a single-date, but right may be.
  if (right_is_single) {
    return 1;
  }

  // Neither is a single date. Compare the upper bounds and return the result.
  return php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_P(left->upper_bound),
                                  PHP5TO7_ZVAL_MAYBE_P(right->upper_bound) TSRMLS_CC);
}

static unsigned
php_driver_date_range_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_date_range *self = PHP_DRIVER_GET_DATE_RANGE(obj);
  unsigned hashv = 0;
  php_driver_date_range_bound *bound;

  bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(self->lower_bound));

  hashv = php_driver_combine_hash(hashv, (unsigned) bound->precision);
  hashv = php_driver_combine_hash(hashv, (unsigned) bound->time_ms);

  if (Z_TYPE_P(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound)) == IS_OBJECT) {
    bound = PHP_DRIVER_GET_DATE_RANGE_BOUND(PHP5TO7_ZVAL_MAYBE_P(self->upper_bound));
    hashv = php_driver_combine_hash(hashv, (unsigned) bound->precision);
    hashv = php_driver_combine_hash(hashv, (unsigned) bound->time_ms);
  }

  return hashv;
}

static void
php_driver_date_range_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_date_range *self = PHP5TO7_ZEND_OBJECT_GET(date_range, object);

  /* Clean up */
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->lower_bound);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->upper_bound);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_date_range_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_date_range *self = PHP5TO7_ZEND_OBJECT_ECALLOC(date_range, ce);
  PHP5TO7_ZEND_OBJECT_INIT(date_range, self, ce);
}

void php_driver_define_DateRange(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DateRange", php_driver_date_range_methods);
  php_driver_date_range_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_custom_ce);
  php_driver_date_range_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_date_range_ce->create_object = php_driver_date_range_new;

  memcpy(&php_driver_date_range_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_date_range_handlers.std.get_properties  = php_driver_date_range_properties;
  php_driver_date_range_handlers.std.compare_objects = php_driver_date_range_compare;
  php_driver_date_range_handlers.std.clone_obj = NULL;
  php_driver_date_range_handlers.hash_value = php_driver_date_range_hash_value;
}
