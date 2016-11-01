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

#include "php_dse.h"
#include "php_dse_types.h"

#include <php_ini.h>
#include <ext/standard/info.h>

ZEND_DECLARE_MODULE_GLOBALS(dse)

static PHP_GINIT_FUNCTION(dse);
static PHP_GSHUTDOWN_FUNCTION(dse);

const zend_function_entry dse_functions[] = {
  PHP_FE_END /* Must be the last line in dse_functions[] */
};

#if ZEND_MODULE_API_NO >= 20050617
static zend_module_dep php_dse_deps[] = {
  ZEND_MOD_REQUIRED("spl")
  ZEND_MOD_END
};
#endif

zend_module_entry dse_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617
  STANDARD_MODULE_HEADER_EX, NULL, php_dse_deps,
#elif ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_DSE_NAME,
  dse_functions,      /* Functions */
  PHP_MINIT(dse),     /* MINIT */
  PHP_MSHUTDOWN(dse), /* MSHUTDOWN */
  PHP_RINIT(dse),     /* RINIT */
  PHP_RSHUTDOWN(dse), /* RSHUTDOWN */
  PHP_MINFO(dse),     /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_DSE_VERSION,
#endif
  PHP_MODULE_GLOBALS(dse),
  PHP_GINIT(dse),
  PHP_GSHUTDOWN(dse),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_DSE
ZEND_GET_MODULE(dse)
#endif

PHP_INI_BEGIN()
  PHP_CASSANDRA_INI_ENTRY_LOG
  PHP_CASSANDRA_INI_ENTRY_LOG_LEVEL
PHP_INI_END()

static PHP_GINIT_FUNCTION(dse)
{
  php_cassandra_ginit();
}

static PHP_GSHUTDOWN_FUNCTION(dse)
{
  php_cassandra_gshutdown();
}

PHP_MINIT_FUNCTION(dse)
{
  REGISTER_INI_ENTRIES();

  int result = php_cassandra_minit(INIT_FUNC_ARGS_PASSTHRU);

  if (result == FAILURE) return result;

  dse_define_Dse(TSRMLS_C);
  dse_define_Session(TSRMLS_C);
  dse_define_FutureSession(TSRMLS_C);
  dse_define_FutureGraphResultSet(TSRMLS_C);
  dse_define_DefaultSession(TSRMLS_C);
  dse_define_Cluster(TSRMLS_C);
  dse_define_DefaultCluster(TSRMLS_C);
  dse_define_ClusterBuilder(TSRMLS_C);
  dse_define_DefaultClusterBuilder(TSRMLS_C);

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(dse)
{
  /* UNREGISTER_INI_ENTRIES(); */

  return php_cassandra_mshutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);
}

PHP_RINIT_FUNCTION(dse)
{
  return php_cassandra_rinit(INIT_FUNC_ARGS_PASSTHRU);
}

PHP_RSHUTDOWN_FUNCTION(dse)
{
  return php_cassandra_rshutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);
}

PHP_MINFO_FUNCTION(dse)
{
  char buf[256];
  php_info_print_table_start();
  php_info_print_table_header(2, "DSE support", "enabled");

  snprintf(buf, sizeof(buf), "%d.%d.%d%s",
           CASS_VERSION_MAJOR, CASS_VERSION_MINOR, CASS_VERSION_PATCH,
           strlen(CASS_VERSION_SUFFIX) > 0 ? "-" CASS_VERSION_SUFFIX : "");
  php_info_print_table_row(2, "C/C++ driver version", buf);

  snprintf(buf, sizeof(buf), "%d", CASSANDRA_G(persistent_clusters));
  php_info_print_table_row(2, "Persistent Clusters", buf);

  snprintf(buf, sizeof(buf), "%d", CASSANDRA_G(persistent_sessions));
  php_info_print_table_row(2, "Persistent Sessions", buf);

  php_info_print_table_end();

  DISPLAY_INI_ENTRIES();
}
