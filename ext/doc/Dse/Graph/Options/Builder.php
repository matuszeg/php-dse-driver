<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph\Options;

/**
 * A builder for fluent configuration of graph options;
 *
 * @see Dse::graphOptions()
 * @see Cluster\Builder::withGraphOptions()
 */
final class Builder {

    /**
     * Constructs a graph options object that can be used to construct
     * a cluster with graph support.
     *
     * @return Graph\Options
     *
     * @see Cluster\Builder::withGraphOptions()
     */
    public function build() { }

    /**
     * Set the graph language. Default: "gremlin-groovy"
     *
     * @param string $language Graph language
     *
     * @return Graph\Options\Builder self
     */
    public function withGraphLanguage($language) { }

    /**
     * Set the graph source. This should use the default: "g" unless running
     * analytics (OLAP) query then it should use "a".
     *
     * @param string $source Graph source
     *
     * @return Graph\Options\Builder self
     */
    public function withGraphSource($source) { }

    /**
     * Set the graph name. This is required unless a per query graph name is
     * used.
     *
     * @param string $name Graph name
     *
     * @return Graph\Options\Builder self
     */
    public function withGraphName($name) { }

    /**
     * Set the read consistency of graph queries. Default: `Dse::CONSISTENCY_ONE`
     *
     * @param int $consistency Read consistency
     *
     * @return Graph\Options\Builder self
     */
    public function withReadConsistency($consistency) { }

    /**
     * Set the write consistency of graph queries. Default: `Dse::CONSISTENCY_QUORUM`
     *
     * @param int $consistency Write consistency
     *
     * @return Graph\Options\Builder self
     */
    public function withWriteConsistency($consistency) { }

    /**
     * Set the request time of graph queries in seconds. Default: `0` (no timeout)
     *
     * @param double $request_timeout Request timeout in seconds
     *
     * @return Graph\Options\Builder self
     */
    public function withRequestTimeout($request_timeout) { }

}
