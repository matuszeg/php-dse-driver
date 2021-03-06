/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_UTIL_TYPES_H
#define PHP_DRIVER_UTIL_TYPES_H

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

php5to7_zval php_driver_type_from_data_type(const CassDataType *data_type TSRMLS_DC);

int php_driver_type_validate(zval *object, const char *object_name TSRMLS_DC);
int php_driver_type_compare(php_driver_type *type1, php_driver_type *type2 TSRMLS_DC);
void php_driver_type_string(php_driver_type *type, smart_str *smart TSRMLS_DC);

php5to7_zval php_driver_type_scalar(CassValueType type TSRMLS_DC);
const char* php_driver_scalar_type_name(CassValueType type TSRMLS_DC);

php5to7_zval php_driver_type_set(zval *value_type TSRMLS_DC);
php5to7_zval php_driver_type_set_from_value_type(CassValueType type TSRMLS_DC);

php5to7_zval php_driver_type_collection(zval *value_type TSRMLS_DC);
php5to7_zval php_driver_type_collection_from_value_type(CassValueType type TSRMLS_DC);

php5to7_zval php_driver_type_map(zval *key_type,
                                    zval *value_type TSRMLS_DC);
php5to7_zval php_driver_type_map_from_value_types(CassValueType key_type,
                                                  CassValueType value_type TSRMLS_DC);

php5to7_zval php_driver_type_tuple(TSRMLS_D);

php5to7_zval php_driver_type_user_type(TSRMLS_D);

php5to7_zval php_driver_type_custom(const char *name,
                                    size_t name_length TSRMLS_DC);

int php_driver_parse_column_type(const char   *validator,
                                 size_t        validator_len,
                                 int          *reversed_out,
                                 int          *frozen_out,
                                 php5to7_zval *type_out TSRMLS_DC);

void php_driver_scalar_init(INTERNAL_FUNCTION_PARAMETERS);

void php_driver_int64_to_string(zval *result, cass_int64_t value);

#endif /* PHP_DRIVER_UTIL_TYPES_H */
