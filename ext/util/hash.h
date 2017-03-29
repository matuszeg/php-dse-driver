/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_HASH_H
#define PHP_DRIVER_HASH_H

#define uthash_malloc(sz) emalloc(sz)
#define uthash_free(ptr,sz) efree(ptr)

#define HASH_FUNCTION(key,keylen,num_bkts,hashv,bkt)                \
  hashv = php_driver_value_hash((zval*)key TSRMLS_CC); \
  bkt = (hashv) & (num_bkts - 1U)
#define HASH_KEYCOMPARE(a, b, len) \
  php_driver_value_compare((zval*)a, (zval*)b TSRMLS_CC)

#undef HASH_ADD /* Previously defined in Zend/zend_hash.h */

#include "util/uthash.h"

#define HASH_FIND_ZVAL(head, zvptr, out) \
    HASH_FIND(hh, head, zvptr, 0, out)

#define HASH_ADD_ZVAL(head, fieldname, add) \
   HASH_ADD_KEYPTR(hh, head, PHP5TO7_ZVAL_MAYBE_P(((add)->fieldname)), 0, add)

struct php_driver_map_entry_ {
  php5to7_zval key;
  php5to7_zval value;
  UT_hash_handle hh;
};

struct php_driver_set_entry_ {
  php5to7_zval value;
  UT_hash_handle hh;
};

#define PHP_DRIVER_COMPARE(a, b) ((a) < (b) ? -1 : (a) > (b))

unsigned php_driver_value_hash(zval* zvalue TSRMLS_DC);
int php_driver_value_compare(zval* zvalue1, zval* zvalue2 TSRMLS_DC);
int php_driver_data_compare(const void* a, const void* b TSRMLS_DC);

unsigned php_driver_mpz_hash(unsigned seed, mpz_t n);

static inline unsigned php_driver_bigint_hash(cass_int64_t value) {
  return (unsigned)(value ^ (value >> 32));
}

static inline unsigned php_driver_combine_hash(unsigned seed, unsigned  hashv) {
  return seed ^ (hashv + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

static inline cass_int32_t
php_driver_hash_float_to_bits(cass_float_t value) {
  cass_int32_t bits;
  if (zend_isnan(value)) return 0x7fc00000; /* A canonical NaN value */
  memcpy(&bits, &value, sizeof(cass_int32_t));
  return bits;
}

static inline int
php_driver_hash_float_compare(cass_float_t v1, cass_float_t v2) {
  cass_int32_t bits1, bits2;

  if (v1 < v2) return -1;
  if (v1 > v2) return  1;

  bits1 = php_driver_hash_float_to_bits(v1);
  bits2 = php_driver_hash_float_to_bits(v2);

  /* Handle NaNs and negative and positive 0.0 */
  return bits1 < bits2 ? -1 : bits1 > bits2;
}

static inline cass_int64_t
php_driver_hash_double_to_bits(cass_double_t value) {
  cass_int64_t bits;
  if (zend_isnan(value)) return 0x7ff8000000000000LL; /* A canonical NaN value */
  memcpy(&bits, &value, sizeof(cass_int64_t));
  return bits;
}

static inline unsigned
php_driver_double_hash(cass_double_t value) {
  return php_driver_bigint_hash(php_driver_hash_double_to_bits(value));
}

static inline int
php_driver_hash_double_compare(cass_double_t v1, cass_double_t v2) {
  cass_int64_t bits1, bits2;

  if (v1 < v2) return -1;
  if (v1 > v2) return  1;

  bits1 = php_driver_hash_double_to_bits(v1);
  bits2 = php_driver_hash_double_to_bits(v2);

  /* Handle NaNs and negative and positive 0.0 */
  return bits1 < bits2 ? -1 : bits1 > bits2;
}

#endif /* PHP_DRIVER_HASH_H */
