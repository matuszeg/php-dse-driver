#ifndef PHP_DSE_H
#define PHP_DSE_H

#include <dse.h>

#include "php_cassandra_shared.h"

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "version.h"

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

void dse_define_Session(TSRMLS_D);

PHP_MINIT_FUNCTION(dse);
PHP_MSHUTDOWN_FUNCTION(dse);
PHP_RINIT_FUNCTION(dse);
PHP_RSHUTDOWN_FUNCTION(dse);
PHP_MINFO_FUNCTION(dse);

#endif /* PHP_DSE_H */
