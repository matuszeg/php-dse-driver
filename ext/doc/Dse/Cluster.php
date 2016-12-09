<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
abstract class Cluster implements Cassandra\Cluster {

    /**
     * @return mixed
     */
    public abstract function connect($keyspace);

    /**
     * @return mixed
     */
    public abstract function connectAsync($keyspace);

}
