<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Default cluster implementation.
 *
 * @see Cassandra\Cluster
 */
final class DefaultCluster implements Cluster {

    /**
     * {@inheritDoc}
     *
     * @param string $keyspace Optional keyspace name
     * @param int $timeout Optional timeout
     * @return Session Session instance
     */
    public function connect($keyspace, $timeout) { }

    /**
     * {@inheritDoc}
     *
     * @param string $keyspace Optional keyspace name
     * @return Future A Future Session instance
     */
    public function connectAsync($keyspace) { }

}
