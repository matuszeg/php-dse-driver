/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/math.h"

php_driver_ref*
php_driver_new_peref(void *data, php_driver_free_function destructor, int persistent)
{
  php_driver_ref *ref = (php_driver_ref*) pemalloc(sizeof(php_driver_ref), persistent);

  ref->data     = data;
  ref->destruct = destructor;
  ref->count    = 1;

  return ref;
}

void
php_driver_del_peref(php_driver_ref **ref_ptr, int persistent)
{
  php_driver_ref *ref = *ref_ptr;
  if (ref) {
    ref->count--;

    if (ref->count <= 0) {
      ref->destruct(ref->data);
      ref->data = NULL;
      pefree(ref, persistent);
      *ref_ptr = NULL;
    }
  }
}
