/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_PARAMS
#define PHP_DRIVER_PARAMS

int php_driver_get_int64_param(zval* value,
                               const char* param_name,
                               cass_int64_t min,
                               cass_int64_t max,
                               const char* extra_supported_types,
                               cass_int64_t *destination TSRMLS_DC);

#endif // PHP_DRIVER_PARAMS