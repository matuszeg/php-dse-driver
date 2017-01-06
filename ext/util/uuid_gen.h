/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_UTIL_UUID_GEN_H
#define PHP_DRIVER_UTIL_UUID_GEN_H

void php_driver_uuid_generate_random(CassUuid *out TSRMLS_DC);
void php_driver_uuid_generate_time(CassUuid *out TSRMLS_DC);
void php_driver_uuid_generate_from_time(long timestamp, CassUuid *out TSRMLS_DC);

#endif /* PHP_DRIVER_UTIL_UUID_GEN_H */
