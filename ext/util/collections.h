/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_UTIL_COLLECTIONS_H
#define PHP_DRIVER_UTIL_COLLECTIONS_H

int php_driver_validate_object(zval* object, zval* ztype TSRMLS_DC);
int php_driver_value_type(char* type, CassValueType* value_type TSRMLS_DC);

int php_driver_collection_from_set(php_driver_set* set, CassCollection** collection_ptr TSRMLS_DC);
int php_driver_collection_from_collection(php_driver_collection* coll, CassCollection** collection_ptr TSRMLS_DC);
int php_driver_collection_from_map(php_driver_map* map, CassCollection** collection_ptr TSRMLS_DC);

int php_driver_tuple_from_tuple(php_driver_tuple *tuple, CassTuple **output TSRMLS_DC);

int php_driver_user_type_from_user_type_value(php_driver_user_type_value *user_type_value, CassUserType **output TSRMLS_DC);

#endif /* PHP_DRIVER_UTIL_COLLECTIONS_H */
