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
final class DefaultSchema implements Schema {

    /**
     * {@inheritDoc}
     * @param string $name Name of the keyspace to get
     * @return Dse\Keyspace Keyspace instance or null
     */
    public function keyspace($name) { }

    /**
     * {@inheritDoc}
     * @return array An array of `Dse\Keyspace` instances.
     */
    public function keyspaces() { }

    /**
     * {@inheritDoc}
     * @return int Version of the schema.
     */
    public function version() { }

}
