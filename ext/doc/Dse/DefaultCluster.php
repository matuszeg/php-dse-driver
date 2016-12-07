<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
final class DefaultCluster implements Cluster {

    /**
     * @param mixed $keyspace
     * @param mixed $timeout
     * @return mixed
     */
    public function connect($keyspace, $timeout) { }

    /**
     * @param mixed $keyspace
     * @return mixed
     */
    public function connectAsync($keyspace) { }

}
