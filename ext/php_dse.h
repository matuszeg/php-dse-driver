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

#define CPP_DRIVER_VERSION(major, minor, patch) \
  (((major) << 16) + ((minor) << 8) + (patch))

#define CPP_DSE_DRIVER_VERSION(major, minor, patch) \
  CPP_DRIVER_VERSION(major, minor, path)

#define CURRENT_CPP_DRIVER_VERSION \
  CPP_DRIVER_VERSION(CASS_VERSION_MAJOR, CASS_VERSION_MINOR, CASS_VERSION_PATCH)

#define CURRENT_CPP_DSE_DRIVER_VERSION \
  CPP_DSE_DRIVER_VERSION(DSE_VERSION_MAJOR, DSE_VERSION_MINOR, DSE_VERSION_PATCH)

#define PHP_DSE_DEFAULT_LOG       "dse.log"
#define PHP_DSE_DEFAULT_LOG_LEVEL "ERROR"

#define PHP_DSE_INI_ENTRY_LOG \
  PHP_INI_ENTRY("dse.log",PHP_DSE_DEFAULT_LOG, PHP_INI_ALL, OnUpdateLog)

#define PHP_DSE_INI_ENTRY_LOG_LEVEL \
  PHP_INI_ENTRY("dse.log_level", PHP_DSE_DEFAULT_LOG_LEVEL, PHP_INI_ALL, OnUpdateLogLevel)

PHP_INI_MH(OnUpdateLog);
PHP_INI_MH(OnUpdateLogLevel);

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
