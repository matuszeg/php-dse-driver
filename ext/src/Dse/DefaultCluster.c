#include "php_dse.h"
#include "php_dse_types.h"
#include "util/future.h"

#include "Cassandra/Cluster.h"
#include "Graph/Options.h"

zend_class_entry *dse_default_cluster_ce = NULL;

PHP_METHOD(DseDefaultCluster, connect)
{
  char *keyspace = NULL;
  php5to7_size keyspace_len;
  zval *timeout = NULL;
  dse_cluster *self;
  dse_session *session;
  cassandra_cluster_base *cluster_base = NULL;
  cassandra_session_base *session_base = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sz", &keyspace, &keyspace_len, &timeout) == FAILURE) {
    return;
  }

  self = PHP_DSE_GET_CLUSTER(getThis());
  cluster_base = &self->base;

  object_init_ex(return_value, dse_default_session_ce);
  session = PHP_DSE_GET_SESSION(return_value);
  session_base = &session->base;

  session_base->default_consistency = cluster_base->default_consistency;
  session_base->default_page_size   = cluster_base->default_page_size;
  session_base->persist             = cluster_base->persist;

  php_dse_graph_options_clone(&session->graph_options, &self->graph_options);

  if (!PHP5TO7_ZVAL_IS_UNDEF(session_base->default_timeout)) {
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(session_base->default_timeout),
                      PHP5TO7_ZVAL_MAYBE_P(cluster_base->default_timeout));
  }

  php_cassandra_cluster_connect(cluster_base,
                                keyspace, keyspace_len,
                                timeout,
                                session_base TSRMLS_CC);

}

PHP_METHOD(DseDefaultCluster, connectAsync)
{
  char *keyspace = NULL;
  php5to7_size keyspace_len;
  cassandra_cluster_base *cluster = NULL;
  cassandra_future_session_base *future = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &keyspace, &keyspace_len) == FAILURE) {
    return;
  }

  cluster = &PHP_DSE_GET_CLUSTER(getThis())->base;

  object_init_ex(return_value, dse_future_session_ce);
  future = &PHP_DSE_GET_FUTURE_SESSION(return_value)->base;

  future->persist = cluster->persist;

  php_cassandra_cluster_connect_async(cluster,
                                      keyspace, keyspace_len,
                                      future TSRMLS_CC);

}

ZEND_BEGIN_ARG_INFO_EX(arginfo_connect, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_connectAsync, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry dse_default_cluster_methods[] = {
  PHP_ME(DseDefaultCluster, connect, arginfo_connect, ZEND_ACC_PUBLIC)
  PHP_ME(DseDefaultCluster, connectAsync, arginfo_connectAsync, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers dse_default_cluster_handlers;

static HashTable *
php_dse_default_cluster_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_dse_default_cluster_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_dse_default_cluster_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  dse_cluster *self = PHP5TO7_ZEND_OBJECT_GET(dse_cluster, object);

  php_cassandra_cluster_destroy(&self->base);
  php_dse_graph_options_destroy(&self->graph_options);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_dse_default_cluster_new(zend_class_entry *ce TSRMLS_DC)
{
  dse_cluster *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(dse_cluster, ce);

  php_cassandra_cluster_init(&self->base);
  php_dse_graph_options_init(&self->graph_options);

  PHP5TO7_ZEND_OBJECT_INIT_EX(dse_cluster, dse_default_cluster, self, ce);
}

void dse_define_DefaultCluster(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\DefaultCluster", dse_default_cluster_methods);
  dse_default_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(dse_default_cluster_ce TSRMLS_CC, 1, dse_cluster_ce);
  dse_default_cluster_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  dse_default_cluster_ce->create_object = php_dse_default_cluster_new;

  memcpy(&dse_default_cluster_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  dse_default_cluster_handlers.get_properties  = php_dse_default_cluster_properties;
  dse_default_cluster_handlers.compare_objects = php_dse_default_cluster_compare;
}
