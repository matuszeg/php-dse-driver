#include "php_dse.h"
#include "php_dse_types.h"

#include "Options.h"

void
php_dse_graph_options_init(dse_graph_options *options)
{
  options->graph_language = NULL;
  options->graph_source = NULL;
  options->graph_name = NULL;
  options->read_consistency = CASS_CONSISTENCY_UNKNOWN;
  options->write_consistency = CASS_CONSISTENCY_UNKNOWN;
  options->request_timeout = -1;
}

void
php_dse_graph_options_destroy(dse_graph_options *options)
{
  if (options->graph_language) {
    efree(options->graph_language);
  }
  if (options->graph_source) {
    efree(options->graph_source);
  }
  if (options->graph_name) {
    efree(options->graph_name);
  }
}

void
php_dse_graph_options_clone(dse_graph_options *dst, dse_graph_options *src)
{
  if (src->graph_language) {
    dst->graph_language = estrdup(dst->graph_language);
  }
  if (src->graph_source) {
    dst->graph_source = estrdup(dst->graph_source);
  }
  if (src->graph_name) {
    dst->graph_name = estrdup(dst->graph_name);
  }
  dst->read_consistency = src->read_consistency;
  dst->write_consistency = src->write_consistency;
  dst->request_timeout = src->request_timeout;
}
