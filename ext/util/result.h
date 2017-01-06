/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_RESULT_H
#define PHP_DRIVER_RESULT_H

int php_driver_value(const CassValue* value, const CassDataType* data_type, php5to7_zval *out TSRMLS_DC);

int php_driver_get_keyspace_field(const CassKeyspaceMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);
int php_driver_get_table_field(const CassTableMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);
int php_driver_get_column_field(const CassColumnMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);

int php_driver_get_result(const CassResult *result, php5to7_zval *out TSRMLS_DC);


#endif /* PHP_DRIVER_RESULT_H */
