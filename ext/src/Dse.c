#include "php_dse.h"
#include "php_dse_types.h"

zend_class_entry *dse_ce = NULL;

PHP_METHOD(Dse, cluster)
{
  object_init_ex(return_value, dse_default_cluster_builder_ce);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry Dse_methods[] = {
  PHP_ME(Dse, cluster, arginfo_none,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

void dse_define_Dse(TSRMLS_D)
{
  zend_class_entry ce;
  char buf[64];

  INIT_CLASS_ENTRY(ce, "Dse", Dse_methods);
  dse_ce = zend_register_internal_class_ex(&ce, cassandra_ce TSRMLS_CC);

  zend_declare_class_constant_string(dse_ce, ZEND_STRL("VERSION"), PHP_CASSANDRA_VERSION_FULL TSRMLS_CC);

  snprintf(buf, sizeof(buf), "%d.%d.%d%s",
           CASS_VERSION_MAJOR, CASS_VERSION_MINOR, CASS_VERSION_PATCH,
           strlen(CASS_VERSION_SUFFIX) > 0 ? "-" CASS_VERSION_SUFFIX : "");
  zend_declare_class_constant_string(dse_ce, ZEND_STRL("CPP_DRIVER_VERSION"), buf TSRMLS_CC);
}
