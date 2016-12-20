<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a schema
 */
interface Schema {

    /**
     * Returns a Keyspace instance by name.
     * @param string $name Name of the keyspace to get
     * @return Cassandra\Keyspace Keyspace instance or null
     */
    public function keyspace($name);

    /**
     * Returns all keyspaces defined in the schema.
     * @return array An array of `Cassandra\Keyspace` instances.
     */
    public function keyspaces();

}
