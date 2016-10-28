#include "php_dse.h"
#include "php_dse_types.h"
#include "util/consistency.h"

#include "Cassandra/Cluster/Builder.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *dse_default_cluster_builder_ce = NULL;

PHP_METHOD(DseDefaultClusterBuilder, build)
{
  cassandra_cluster_base *cluster = NULL;

  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  cassandra_cluster_builder_base *builder = &self->base;

  object_init_ex(return_value, cassandra_default_cluster_ce);
  cluster = &PHP_CASSANDRA_GET_CLUSTER(return_value)->base;

  cluster->persist             = builder->persist;
  cluster->default_consistency = builder->default_consistency;
  cluster->default_page_size   = builder->default_page_size;

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(cluster->default_timeout),
                    PHP5TO7_ZVAL_MAYBE_P(builder->default_timeout));

  php_cassandra_cluster_builder_generate_hash_key(builder,
                                                  &cluster->hash_key,
                                                  &cluster->hash_key_len);

  cluster->cluster = php_cassandra_cluster_builder_get_cache(builder,
                                                             cluster->hash_key,
                                                             cluster->hash_key_len);

  if (!cluster->cluster) {
    cluster->cluster = cass_cluster_new_dse();
    php_cassandra_cluster_builder_build(builder,
                                        cluster->cluster);

    if (self->plaintext_username) {
      cass_cluster_set_dse_plaintext_authenticator(cluster->cluster,
                                                   self->plaintext_username,
                                                   self->plaintext_password);
    }

    if (self->gssapi_service) {
      cass_cluster_set_dse_gssapi_authenticator(cluster->cluster,
                                                self->gssapi_service,
                                                self->gssapi_principal);
    }

    php_cassandra_cluster_builder_add_cache(builder,
                                            cluster->hash_key,
                                            cluster->hash_key_len,
                                            cluster->cluster);
  }
}

PHP_METHOD(DseDefaultClusterBuilder, withDefaultConsistency)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_default_consistency(&self->base,
                                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withDefaultPageSize)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_default_page_size(&self->base,
                                                       INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withDefaultTimeout)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_with_default_timeout(&self->base,
                                                          INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withContactPoints)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_with_contact_points(&self->base,
                                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withPort)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_port(&self->base,
                                          INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withRoundRobinLoadBalancingPolicy)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_round_robin_lb_policy(&self->base,
                                                           INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withDatacenterAwareRoundRobinLoadBalancingPolicy)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_dc_aware_lb_policy(&self->base,
                                                        INTERNAL_FUNCTION_PARAM_PASSTHRU);
}


PHP_METHOD(DseDefaultClusterBuilder, withBlackListHosts)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_blacklist_hosts(&self->base,
                                                     INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withWhiteListHosts)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_whitelist_hosts(&self->base,
                                                     INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withBlackListDCs)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_blacklist_dcs(&self->base,
                                                   INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withWhiteListDCs)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_whitelist_dcs(&self->base,
                                                   INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withTokenAwareRouting)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_token_aware_routing(&self->base,
                                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withCredentials)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_credentials(&self->base,
                                                 INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withConnectTimeout)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_connect_timeout(&self->base,
                                                     INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withRequestTimeout)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_request_timeout(&self->base,
                                                     INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withSSL)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_ssl(&self->base,
                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withPersistentSessions)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_persistent_sessions(&self->base,
                                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withProtocolVersion)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_protocol_version(&self->base,
                                                      INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withIOThreads)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_io_threads(&self->base,
                                                INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withConnectionsPerHost)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_connections_per_host(&self->base,
                                                          INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withReconnectInterval)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_reconnect_interval(&self->base,
                                                        INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withLatencyAwareRouting)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_latency_aware_routing(&self->base,
                                                           INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withTCPNodelay)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_tcp_nodelay(&self->base,
                                                 INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withTCPKeepalive)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_tcp_keepalive(&self->base,
                                                   INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withRetryPolicy)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_retry_policy(&self->base,
                                                  INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withTimestampGenerator)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_timestamp_generator(&self->base,
                                                         INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withSchemaMetadata)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());
  php_cassandra_cluster_builder_with_schema_metadata(&self->base,
                                                     INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(DseDefaultClusterBuilder, withPlaintextAuthenticator)
{
  zval *username = NULL;
  zval *password = NULL;

  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &username, &password) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(username) != IS_STRING) {
    INVALID_ARGUMENT(username, "a string");
  }

  if (Z_TYPE_P(password) != IS_STRING) {
    INVALID_ARGUMENT(password, "a string");
  }

  if (self->plaintext_username) {
    efree(self->plaintext_username);
    efree(self->plaintext_password);
  }

  self->plaintext_username = estrndup(Z_STRVAL_P(username), Z_STRLEN_P(username));
  self->plaintext_password = estrndup(Z_STRVAL_P(password), Z_STRLEN_P(password));

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(DseDefaultClusterBuilder, withGssapiAuthenticator)
{
  zval *service = NULL;
  zval *principal = NULL;

  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &service, &principal) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(service) != IS_STRING) {
    INVALID_ARGUMENT(service, "a string");
  }

  if (Z_TYPE_P(principal) != IS_STRING) {
    INVALID_ARGUMENT(principal, "a string");
  }

  if (self->gssapi_service) {
    efree(self->gssapi_service);
    efree(self->gssapi_principal);
  }

  self->gssapi_service = estrndup(Z_STRVAL_P(service), Z_STRLEN_P(service));
  self->gssapi_principal = estrndup(Z_STRVAL_P(principal), Z_STRLEN_P(principal));

  RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_consistency, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, consistency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_page_size, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, pageSize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_contact_points, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_port, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dc_aware, 0, ZEND_RETURN_VALUE, 3)
  ZEND_ARG_INFO(0, localDatacenter)
  ZEND_ARG_INFO(0, hostPerRemoteDatacenter)
  ZEND_ARG_INFO(0, useRemoteDatacenterForLocalConsistencies)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_blacklist_nodes, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, hosts)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_whitelist_nodes, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, hosts)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_blacklist_dcs, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, dcs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_whitelist_dcs, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, dcs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enabled, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, enabled)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_credentials, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, username)
  ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ssl, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\SSLOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_version, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, version)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_count, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_connections, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, core)
  ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_interval, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, interval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_delay, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, delay)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_retry_policy, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, policy, Cassandra\\RetryPolicy, 0)
ZEND_END_ARG_INFO()

  ZEND_BEGIN_ARG_INFO_EX(arginfo_timestamp_gen, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, generator, Cassandra\\TimestampGenerator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_plaintext_authenticator, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, username)
  ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gssapi_authenticator, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, service)
  ZEND_ARG_INFO(0, principal)
ZEND_END_ARG_INFO()

static zend_function_entry dse_default_cluster_builder_methods[] = {
  PHP_ME(DseDefaultClusterBuilder, build, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withDefaultConsistency, arginfo_consistency, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withDefaultPageSize, arginfo_page_size, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withDefaultTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withContactPoints, arginfo_contact_points, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withPort, arginfo_port, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withRoundRobinLoadBalancingPolicy, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withDatacenterAwareRoundRobinLoadBalancingPolicy, arginfo_dc_aware, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withBlackListHosts, arginfo_blacklist_nodes, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withWhiteListHosts, arginfo_whitelist_nodes, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withBlackListDCs, arginfo_blacklist_dcs, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withWhiteListDCs, arginfo_whitelist_dcs, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withTokenAwareRouting, arginfo_enabled, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withCredentials, arginfo_credentials, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withConnectTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withRequestTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withSSL, arginfo_ssl, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withPersistentSessions, arginfo_enabled, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withProtocolVersion, arginfo_version, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withIOThreads, arginfo_count, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withConnectionsPerHost, arginfo_connections, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withReconnectInterval, arginfo_interval, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withLatencyAwareRouting, arginfo_enabled, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withTCPNodelay, arginfo_enabled, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withTCPKeepalive, arginfo_delay, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withRetryPolicy, arginfo_retry_policy, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withTimestampGenerator, arginfo_timestamp_gen, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withSchemaMetadata, arginfo_enabled, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withPlaintextAuthenticator, arginfo_plaintext_authenticator, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultClusterBuilder, withGssapiAuthenticator, arginfo_gssapi_authenticator, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_default_cluster_builder_handlers;

static HashTable*
php_dse_default_cluster_builder_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_dse_default_cluster_builder_properties(zval *object TSRMLS_DC)
{
  dse_cluster_builder *self = PHP_DSE_GET_CLUSTER_BUILDER(object);
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  php_cassandra_cluster_builder_properties(&self->base, props);

  return props;
}

static int
php_dse_default_cluster_builder_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_default_cluster_builder_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_cluster_builder *self =
      PHP5TO7_ZEND_OBJECT_GET(dse_cluster_builder, object);

  php_cassandra_cluster_builder_destroy(&self->base);

  if (self->plaintext_username) {
    efree(self->plaintext_username);
    self->plaintext_username = NULL;
  }

  if (self->plaintext_password) {
    efree(self->plaintext_password);
    self->plaintext_password = NULL;
  }

  if (self->gssapi_service) {
    efree(self->gssapi_service);
    self->gssapi_service = NULL;
  }

  if (self->gssapi_principal) {
    efree(self->gssapi_principal);
    self->gssapi_principal = NULL;
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_default_cluster_builder_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_cluster_builder *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_cluster_builder, ce);

  php_cassandra_cluster_builder_init(&self->base);

  self->plaintext_username = NULL;
  self->plaintext_password = NULL;
  self->gssapi_service = NULL;
  self->gssapi_principal = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_cluster_builder, dse_default_cluster_builder, self, ce);
}

void dse_define_DefaultClusterBuilder(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Cluster\\DefaultBuilder", dse_default_cluster_builder_methods);
  dse_default_cluster_builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_default_cluster_builder_ce TSRMLS_CC, 1, dse_cluster_builder_ce);
  dse_default_cluster_builder_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_default_cluster_builder_ce->create_object = php_dse_default_cluster_builder_new;

  memcpy(&dse_default_cluster_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_default_cluster_builder_handlers.get_properties  = php_dse_default_cluster_builder_properties;
#if PHP_VERSION_ID >= 50400
  dse_default_cluster_builder_handlers.get_gc          = php_dse_default_cluster_builder_gc;
#endif
  dse_default_cluster_builder_handlers.compare_objects = php_dse_default_cluster_builder_compare;
}
