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

// Format string macro for how to emit a coordinate of a Point.
#define COORD_FMT "%.17g"

#if PHP_MAJOR_VERSION >= 7
  #define PHP_DSE_GET_SESSION(obj) php_dse_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_FUTURE_SESSION(obj) php_dse_future_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_CLUSTER(obj) php_dse_cluster_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_CLUSTER_BUILDER(obj) php_dse_cluster_builder_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_STATEMENT(obj) php_dse_graph_statement_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_RESULT(obj) php_dse_graph_result_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_RESULT_SET(obj) php_dse_graph_result_set_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_FUTURE_RESULT_SET(obj) php_dse_graph_future_result_set_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_ELEMENT(obj) php_dse_graph_element_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_PROPERTY(obj) php_dse_graph_property_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_EDGE(obj) php_dse_graph_edge_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_PATH(obj) php_dse_graph_path_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_VERTEX(obj) php_dse_graph_vertex_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(obj) php_dse_graph_vertex_property_object_fetch(Z_OBJ_P(obj))

  #define PHP_DSE_GET_POINT(obj) php_dse_point_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_LINE_STRING(obj) php_dse_line_string_object_fetch(Z_OBJ_P(obj))
  #define PHP_DSE_GET_POLYGON(obj) php_dse_polygon_object_fetch(Z_OBJ_P(obj))
#else
  #define PHP_DSE_GET_SESSION(obj) ((dse_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_FUTURE_SESSION(obj) ((dse_future_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_CLUSTER(obj) ((dse_cluster *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_CLUSTER_BUILDER(obj) ((dse_cluster_builder *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_STATEMENT(obj) ((dse_graph_statement *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_RESULT(obj) ((dse_graph_result *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_RESULT_SET(obj) ((dse_graph_result_set *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_FUTURE_RESULT_SET(obj) ((dse_graph_future_result_set *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_ELEMENT(obj) ((dse_graph_element *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_PROPERTY(obj) ((dse_graph_property *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_EDGE(obj) ((dse_graph_edge *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_PATH(obj) ((dse_graph_path *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_VERTEX(obj) ((dse_graph_vertex *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_GRAPH_VERTEX_PROPERTY(obj) ((dse_graph_vertex_property *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_POINT(obj) ((dse_point *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_LINE_STRING(obj) ((dse_line_string *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DSE_GET_POLYGON(obj) ((dse_polygon *)zend_object_store_get_object((obj) TSRMLS_CC))
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

PHP_DSE_BEGIN_OBJECT_TYPE(graph_statement)
  char *query;
PHP_DSE_END_OBJECT_TYPE(graph_statement)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_result)
  DseGraphResultType type;
  php5to7_zval value;
PHP_DSE_END_OBJECT_TYPE(graph_result)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_result_set)
  php5to7_zval results;
PHP_DSE_END_OBJECT_TYPE(graph_result_set)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_future_result_set)
  php5to7_zval result_set;
  CassFuture *future;
PHP_DSE_END_OBJECT_TYPE(graph_future_result_set)

typedef struct {
  php5to7_zval id;
  char *label;
  php5to7_zval properties;
} dse_graph_element_base;

typedef struct {
  char *name;
  php5to7_zval value;
  php5to7_zval parent;
} dse_graph_property_base;

PHP_DSE_BEGIN_OBJECT_TYPE(graph_element)
  dse_graph_element_base base;
PHP_DSE_END_OBJECT_TYPE(graph_element)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_property)
  dse_graph_property_base base;
PHP_DSE_END_OBJECT_TYPE(graph_property)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_edge)
  dse_graph_element_base element;
  php5to7_zval in_v;
  char *in_v_label;
  php5to7_zval out_v;
  char *out_v_label;
PHP_DSE_END_OBJECT_TYPE(graph_edge)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_path)
  php5to7_zval labels;
  php5to7_zval objects;
PHP_DSE_END_OBJECT_TYPE(graph_path)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_vertex)
  dse_graph_element_base element;
PHP_DSE_END_OBJECT_TYPE(graph_vertex)

PHP_DSE_BEGIN_OBJECT_TYPE(graph_vertex_property)
  dse_graph_element_base element;
  dse_graph_property_base property;
PHP_DSE_END_OBJECT_TYPE(graph_vertex_property)

PHP_DSE_BEGIN_OBJECT_TYPE(point)
  double x;
  double y;
PHP_DSE_END_OBJECT_TYPE(point)

PHP_DSE_BEGIN_OBJECT_TYPE(line_string)
  // array of points
  php5to7_zval points;
PHP_DSE_END_OBJECT_TYPE(line_string)

PHP_DSE_BEGIN_OBJECT_TYPE(polygon)
  // LineString
  php5to7_zval exterior_ring;
  // array of LineString
  php5to7_zval interior_rings;
PHP_DSE_END_OBJECT_TYPE(polygon)

extern PHP_DRIVER_API zend_class_entry *dse_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_future_session_ce;
extern PHP_DRIVER_API zend_class_entry *dse_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *dse_cluster_builder_ce;
extern PHP_DRIVER_API zend_class_entry *dse_default_cluster_builder_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_statement_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_simple_statement_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_result_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_result_set_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_future_result_set_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_element_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_property_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_edge_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_path_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_vertex_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_vertex_property_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_element_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_property_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_edge_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_path_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_vertex_ce;
extern PHP_DRIVER_API zend_class_entry *dse_graph_default_vertex_property_ce;
extern PHP_DRIVER_API zend_class_entry *dse_point_ce;
extern PHP_DRIVER_API zend_class_entry *dse_line_string_ce;
extern PHP_DRIVER_API zend_class_entry *dse_polygon_ce;

void dse_define_Dse(TSRMLS_D);
void dse_define_Session(TSRMLS_D);
void dse_define_FutureSession(TSRMLS_D);
void dse_define_DefaultSession(TSRMLS_D);
void dse_define_Cluster(TSRMLS_D);
void dse_define_DefaultCluster(TSRMLS_D);
void dse_define_ClusterBuilder(TSRMLS_D);
void dse_define_DefaultClusterBuilder(TSRMLS_D);
void dse_define_GraphStatement(TSRMLS_D);
void dse_define_GraphSimpleStatement(TSRMLS_D);
void dse_define_GraphResult(TSRMLS_D);
void dse_define_GraphResultSet(TSRMLS_D);
void dse_define_GraphFutureResultSet(TSRMLS_D);
void dse_define_GraphElement(TSRMLS_D);
void dse_define_GraphProperty(TSRMLS_D);
void dse_define_GraphEdge(TSRMLS_D);
void dse_define_GraphPath(TSRMLS_D);
void dse_define_GraphVertex(TSRMLS_D);
void dse_define_GraphVertexProperty(TSRMLS_D);
void dse_define_GraphDefaultElement(TSRMLS_D);
void dse_define_GraphDefaultProperty(TSRMLS_D);
void dse_define_GraphDefaultEdge(TSRMLS_D);
void dse_define_GraphDefaultPath(TSRMLS_D);
void dse_define_GraphDefaultVertex(TSRMLS_D);
void dse_define_GraphDefaultVertexProperty(TSRMLS_D);
void dse_define_Point(TSRMLS_D);
void dse_define_LineString(TSRMLS_D);
void dse_define_Polygon(TSRMLS_D);

#endif /* PHP_DSE_TYPES_H */
