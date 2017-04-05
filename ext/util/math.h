/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_MATH_H
#define PHP_DRIVER_MATH_H

void import_twos_complement(cass_byte_t *data, size_t size, mpz_t *number);
cass_byte_t *export_twos_complement(mpz_t number, size_t *size);

int php_driver_parse_float(char *in, int in_len, const char* param_name, cass_float_t *number TSRMLS_DC);
int php_driver_parse_double(char* in, int in_len, const char* param_name, cass_double_t* number TSRMLS_DC);
int php_driver_parse_int(char* in, int in_len, const char* param_name, cass_int32_t* number TSRMLS_DC);
int php_driver_parse_bigint(char *in, int in_len, const char* param_name, cass_int64_t *number TSRMLS_DC);
int php_driver_parse_varint(char *in, int in_len, const char* param_name, mpz_t *number TSRMLS_DC);
int php_driver_parse_decimal(char *in, int in_len, const char* param_name, mpz_t *number, long *scale TSRMLS_DC);

void php_driver_format_integer(mpz_t number, char **out, int *out_len);
void php_driver_format_decimal(mpz_t number, long scale, char **out, int *out_len);

#endif /* PHP_DRIVER_MATH_H */
