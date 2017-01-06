/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef PHP_DRIVER_REF_H
#define PHP_DRIVER_REF_H

php_driver_ref *php_driver_new_peref(void *data, php_driver_free_function destructor, int persistent);
void php_driver_del_peref(php_driver_ref **ref_ptr, int persistent);

#define php_driver_new_ref(data, destructor) php_driver_new_peref(data, destructor, 0)
#define php_driver_del_ref(ref) php_driver_del_peref(ref, 0)
#define php_driver_add_ref(ref) (++ref->count, ref)

#endif /* PHP_DRIVER_REF_H */
