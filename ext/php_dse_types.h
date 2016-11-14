#ifndef PHP_DSE_TYPES_H
#define PHP_DSE_TYPES_H

#include "php_cassandra_types.h"

#if PHP_MAJOR_VERSION >= 7
#define PHP_DSE_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct dse_##type_name##_ {

#define PHP_DSE_END_OBJECT_TYPE(type_name) \
    zend_object zval;                            \
  } dse_##type_name;                       \
  static inline dse_##type_name *php_dse_##type_name##_object_fetch(zend_object *obj) { \
    return (dse_##type_name *)((char *)obj - XtOffsetOf(dse_##type_name, zval));        \
  }
#else
#define PHP_DSE_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct dse_##type_name##_ {        \
    zend_object zval;

#define PHP_DSE_END_OBJECT_TYPE(type_name) \
  } dse_##type_name;
#endif

#if PHP_MAJOR_VERSION >= 7
  #define PHP_DSE_GET_SESSION(obj) php_dse_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_FUTURE_SESSION(obj) php_dse_future_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_CLUSTER(obj) php_dse_cluster_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_CLUSTER_BUILDER(obj) php_dse_cluster_builder_object_fetch(Z_OBJ_P(obj))
#else
  #define PHP_DSE_GET_SESSION(obj) ((dse_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_FUTURE_SESSION(obj) ((dse_future_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_CLUSTER(obj) ((dse_cluster *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_CLUSTER_BUILDER(obj) ((dse_cluster_builder *)zend_object_store_get_object((obj) TSRMLS_CC))
#endif

typedef struct {
  char *graph_language;
  char *graph_source;
  char *graph_name;
  CassConsistency read_consistency;
  CassConsistency write_consistency;
  cass_int64_t request_timeout;
} dse_graph_options;

PHP_DSE_BEGIN_OBJECT_TYPE(session)
  cassandra_session_base base;
  dse_graph_options graph_options;
PHP_DSE_END_OBJECT_TYPE(session)

PHP_DSE_BEGIN_OBJECT_TYPE(future_session)
  cassandra_future_session_base base;
  dse_graph_options graph_options;
PHP_DSE_END_OBJECT_TYPE(future_session)

PHP_DSE_BEGIN_OBJECT_TYPE(future_graph_result_set)
  int unused;
PHP_DSE_END_OBJECT_TYPE(future_graph_result_set)

PHP_DSE_BEGIN_OBJECT_TYPE(cluster)
  cassandra_cluster_base base;
  dse_graph_options graph_options;
PHP_DSE_END_OBJECT_TYPE(cluster)

PHP_DSE_BEGIN_OBJECT_TYPE(cluster_builder)
  cassandra_cluster_builder_base base;
  char *plaintext_username;
  char *plaintext_password;
  char *gssapi_service;
  char *gssapi_principal;
  dse_graph_options graph_options;
PHP_DSE_END_OBJECT_TYPE(cluster_builder)

extern PHP_DRIVER_API zend_class_entry *dse_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_future_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_future_graph_future_result_set_ce;
extern PHP_DRIVER_API zend_class_entry *dse_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *dse_cluster_builder_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_cluster_builder_ce;

void dse_define_Dse(TSRMLS_D);
void dse_define_Session(TSRMLS_D);
void dse_define_FutureSession(TSRMLS_D);
void dse_define_FutureGraphResultSet(TSRMLS_D);
void dse_define_DefaultSession(TSRMLS_D);
void dse_define_Cluster(TSRMLS_D);
void dse_define_DefaultCluster(TSRMLS_D);
void dse_define_ClusterBuilder(TSRMLS_D);
void dse_define_DefaultClusterBuilder(TSRMLS_D);

#endif /* PHP_DSE_TYPES_H */
