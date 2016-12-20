<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a keyspace
 */
final class DefaultKeyspace implements Keyspace {

    /**
     * {@inheritDoc}
     * @return string Name
     */
    public function name() { }

    /**
     * {@inheritDoc}
     * @return string Replication class
     */
    public function replicationClassName() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Map Replication options
     */
    public function replicationOptions() { }

    /**
     * {@inheritDoc}
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites() { }

    /**
     * {@inheritDoc}
     * @param string $name Table name
     * @return Cassandra\Table Table instance or null
     */
    public function table($name) { }

    /**
     * {@inheritDoc}
     * @return array An array of `Cassandra\Table` instances
     */
    public function tables() { }

    /**
     * @param mixed $name
     * @return mixed
     */
    public function userType($name) { }

    /**
     * @return mixed
     */
    public function userTypes() { }

    /**
     * @param mixed $name
     * @return mixed
     */
    public function materializedView($name) { }

    /**
     * @return mixed
     */
    public function materializedViews() { }

    /**
     * @param mixed $name
     * @param mixed $...
     * @return mixed
     */
    public function function_($name, ...$params) { }

    /**
     * @return mixed
     */
    public function functions() { }

    /**
     * @param mixed $name
     * @param mixed $...
     * @return mixed
     */
    public function aggregate($name, ...$params) { }

    /**
     * @return mixed
     */
    public function aggregates() { }

}
