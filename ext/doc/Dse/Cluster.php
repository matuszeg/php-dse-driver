<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Cluster object is used to create Sessions.
 */
interface Cluster {

    /**
     * Creates a new Session instance.
     *
     * @param string $keyspace Optional keyspace name
     *
     * @return Session Session instance
     */
    public function connect($keyspace);

    /**
     * Creates a new Session instance.
     *
     * @param string $keyspace Optional keyspace name
     *
     * @return Future A Future Session instance
     */
    public function connectAsync($keyspace);

}
