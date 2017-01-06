/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_BYTES_H
#define PHP_DRIVER_BYTES_H

void php_driver_bytes_to_hex(const char *bin, int len, char **out, int *out_len);

#endif /* PHP_DRIVER_BYTES_H */
