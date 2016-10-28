/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PHP_DSE_H
#define PHP_DSE_H

#include <dse.h>

#include "php_cassandra_shared.h"

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "version.h"

#ifdef PHP_WIN32
#  define PHP_DSE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#  define PHP_DSE_API __attribute__ ((visibility("default")))
#else
#  define PHP_DSE_API
#endif

void dse_define_Session(TSRMLS_D);

PHP_MINIT_FUNCTION(dse);
PHP_MSHUTDOWN_FUNCTION(dse);
PHP_RINIT_FUNCTION(dse);
PHP_RSHUTDOWN_FUNCTION(dse);
PHP_MINFO_FUNCTION(dse);

ZEND_BEGIN_MODULE_GLOBALS(dse)
  int notused;
ZEND_END_MODULE_GLOBALS(dse)

#ifdef ZTS
#  define DSE_G(v) TSRMG(dse_globals_id, zend_dse_globals *, v)
#else
#  define DSE_G(v) (dse_globals.v)
#endif

#endif /* PHP_DSE_H */
