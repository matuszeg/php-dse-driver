/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"

int php_driver_get_consistency(zval *consistency, long *result TSRMLS_DC)
{
  if (consistency && Z_TYPE_P(consistency) == IS_LONG) {
    switch (Z_LVAL_P(consistency)) {
    case CASS_CONSISTENCY_ANY:
    case CASS_CONSISTENCY_ONE:
    case CASS_CONSISTENCY_TWO:
    case CASS_CONSISTENCY_THREE:
    case CASS_CONSISTENCY_QUORUM:
    case CASS_CONSISTENCY_ALL:
    case CASS_CONSISTENCY_LOCAL_QUORUM:
    case CASS_CONSISTENCY_EACH_QUORUM:
    case CASS_CONSISTENCY_SERIAL:
    case CASS_CONSISTENCY_LOCAL_SERIAL:
    case CASS_CONSISTENCY_LOCAL_ONE:
      *result = Z_LVAL_P(consistency);
      break;
    default:
      INVALID_ARGUMENT_VALUE(consistency, "one of " PHP_DRIVER_NAMESPACE "::CONSISTENCY_*", FAILURE);
    }
  } else {
    INVALID_ARGUMENT_VALUE(consistency, "one of " PHP_DRIVER_NAMESPACE "::CONSISTENCY_*", FAILURE);
  }
  return SUCCESS;
}

int php_driver_get_serial_consistency(zval *serial_consistency, long *result TSRMLS_DC)
{
  if (serial_consistency && Z_TYPE_P(serial_consistency) == IS_LONG) {
    switch (Z_LVAL_P(serial_consistency)) {
    case CASS_CONSISTENCY_SERIAL:
    case CASS_CONSISTENCY_LOCAL_SERIAL:
      *result = Z_LVAL_P(serial_consistency);
      break;
    default:
      INVALID_ARGUMENT_VALUE(serial_consistency, "either " PHP_DRIVER_NAMESPACE "::CONSISTENCY_SERIAL or Cassanra::CASS_CONSISTENCY_LOCAL_SERIAL", FAILURE);
    }
  } else {
    INVALID_ARGUMENT_VALUE(serial_consistency, "either " PHP_DRIVER_NAMESPACE "::CONSISTENCY_SERIAL or Cassanra::CASS_CONSISTENCY_LOCAL_SERIAL", FAILURE);
  }
  return SUCCESS;
}
