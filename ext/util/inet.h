/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_UTIL_INET_H
#define PHP_DRIVER_UTIL_INET_H

void php_driver_format_address(CassInet inet, char **out);
int php_driver_parse_ip_address(char *in, CassInet *inet TSRMLS_DC);


#endif /* PHP_DRIVER_UTIL_INET_H */
