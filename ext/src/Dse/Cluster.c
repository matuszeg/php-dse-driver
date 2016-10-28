#include "php_dse.h"

zend_class_entry *dse_cluster_ce = NULL;

static zend_function_entry dse_cluster_methods[] = {
  PHP_FE_END
};

void dse_define_Cluster(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Dse\\Cluster", dse_cluster_methods);
  dse_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  dse_cluster_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
