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
interface Keyspace {

    /**
     * Returns keyspace name
     * @return string Name
     */
    public function name();

    /**
     * Returns replication class name
     * @return string Replication class
     */
    public function replicationClassName();

    /**
     * Returns replication options
     * @return Cassandra\Map Replication options
     */
    public function replicationOptions();

    /**
     * Returns whether the keyspace has durable writes enabled
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites();

    /**
     * Returns a table by name
     * @param string $name Table name
     * @return Cassandra\Table|null Table instance or null
     */
    public function table($name);

    /**
     * Returns all tables defined in this keyspace
     * @return array An array of `Cassandra\Table` instances
     */
    public function tables();

    /**
     * @param mixed $name
     * @return mixed
     */
    public function userType($name);

    /**
     * @return mixed
     */
    public function userTypes();

    /**
     * @param mixed $name
     * @return mixed
     */
    public function materializedView($name);

    /**
     * @return mixed
     */
    public function materializedViews();

    /**
     * @param mixed $name
     * @param mixed $...
     * @return mixed
     */
    public function function_($name, ...$params);

    /**
     * @return mixed
     */
    public function functions();

    /**
     * @param mixed $name
     * @param mixed $...
     * @return mixed
     */
    public function aggregate($name, ...$params);

    /**
     * @return mixed
     */
    public function aggregates();

}
