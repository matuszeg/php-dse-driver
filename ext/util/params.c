/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/math.h"

int php_driver_get_int64_param(zval* value,
                               const char* param_name,
                               cass_int64_t min,
                               cass_int64_t max,
                               const char* extra_supported_types,
                               cass_int64_t *destination TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_LONG) {
    php5to7_long long_value = Z_LVAL_P(value);

    if (long_value > max || long_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, long_value);
      return FAILURE;
    }

    *destination = long_value;
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    double double_value = Z_DVAL_P(value);

    if (double_value > max || double_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", %g given",
        param_name, min, max, double_value);
      return FAILURE;
    }
    *destination = (cass_int64_t) double_value;
  } else if (Z_TYPE_P(value) == IS_STRING) {
    cass_int64_t parsed_big_int;
    if (!php_driver_parse_bigint(Z_STRVAL_P(value), Z_STRLEN_P(value), param_name, &parsed_big_int TSRMLS_CC)) {
      return FAILURE;
    }

    if (parsed_big_int > max || parsed_big_int < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, parsed_big_int);
      return FAILURE;
    }
    *destination = parsed_big_int;
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
    php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
    cass_int64_t bigint_value = bigint->data.bigint.value;

    if (bigint_value > max || bigint_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, bigint_value);
      return FAILURE;
    }

    *destination = bigint_value;
  } else {
    char *buf;
    spprintf(&buf, 0, "a long, a double, a numeric string, %sor a " PHP_DRIVER_NAMESPACE "\\Bigint",
      extra_supported_types ? extra_supported_types : "");
    throw_invalid_argument(value, param_name, buf TSRMLS_CC);
    efree(buf);
    return FAILURE;
  }
  return SUCCESS;
}
