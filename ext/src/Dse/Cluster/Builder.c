#include "php_cassandra.h"
#include "util/consistency.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *dse_cluster_builder_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_plaintext_authenticator, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, username)
  ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gssapi_authenticator, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, service)
  ZEND_ARG_INFO(0, principal)
ZEND_END_ARG_INFO()

static zend_function_entry dse_cluster_builder_methods[] = {
  PHP_ABSTRACT_ME(DseCluster, withPlaintextAuthenticator, arginfo_plaintext_authenticator)
  PHP_ABSTRACT_ME(DseCluster, withGssapiAuthenticator, arginfo_gssapi_authenticator)
  PHP_FE_END
};

void dse_define_ClusterBuilder(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Cluster\\Builder", dse_cluster_builder_methods);
  dse_cluster_builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_cluster_builder_ce TSRMLS_CC, 1, cassandra_cluster_builder_ce);
  dse_cluster_builder_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
