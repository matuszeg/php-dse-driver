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
     *
     * @return string Name
     */
    public function name();

    /**
     * Returns replication class name
     *
     * @return string Replication class
     */
    public function replicationClassName();

    /**
     * Returns replication options
     *
     * @return Map Replication options
     */
    public function replicationOptions();

    /**
     * Returns whether the keyspace has durable writes enabled
     *
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites();

    /**
     * Returns a table by name
     *
     * @param string $name Table name
     *
     * @return Table|null Table instance or null
     */
    public function table($name);

    /**
     * Returns all tables defined in this keyspace
     *
     * @return array An array of `Table` instances
     */
    public function tables();

    /**
     * Get user type by name
     *
     * @param string $name User type name
     *
     * @return Type\UserType|null A user type or null
     */
    public function userType($name);

    /**
     * Get all user types
     *
     * @return array An array of user types
     */
    public function userTypes();

    /**
     * Get materialized view by name
     *
     * @param string $name Materialized view name
     *
     * @return MaterizedView|null A materialized view or null
     */
    public function materializedView($name);

    /**
     * Gets all materialized views
     *
     * @return array An array of materialized views
     */
    public function materializedViews();

    /**
     * Get a function by name and signature
     *
     * @param string $name Function name
     * @param string|Type $params Function arguments
     *
     * @return Function|null A function or null
     */
    public function function_($name, ...$params);

    /**
     * Get all functions
     *
     * @return array An array of functions
     */
    public function functions();

    /**
     * Get an aggregate by name and signature
     *
     * @param string $name Aggregate name
     * @param string|Type $params Aggregate arguments
     *
     * @return Aggregate|null An aggregate or null
     */
    public function aggregate($name, ...$params);

    /**
     * Get all aggregates
     *
     * @return array An array of aggregates
     */
    public function aggregates();

}
