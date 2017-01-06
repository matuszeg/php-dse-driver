/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_USER_TYPE_VALUE_H
#define PHP_DRIVER_USER_TYPE_VALUE_H

int php_driver_user_type_value_set(php_driver_user_type_value *user_type_value,
                                      const char *name, size_t name_length,
                                      zval *object TSRMLS_DC);


#endif /* PHP_DRIVER_USER_TYPE_VALUE_H */
