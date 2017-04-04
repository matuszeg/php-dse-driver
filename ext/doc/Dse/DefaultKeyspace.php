<?php

/*
  Copyright (c) 2017 DataStax, Inc.
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
     *
     * @return string Name
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string Replication class
     */
    public function replicationClassName() { }

    /**
     * {@inheritDoc}
     *
     * @return Map Replication options
     */
    public function replicationOptions() { }

    /**
     * {@inheritDoc}
     *
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name Table name
     *
     * @return Table Table instance or null
     */
    public function table($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array An array of `Table` instances
     */
    public function tables() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return Type\UserType|null {@inheritDoc}
     */
    public function userType($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function userTypes() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return MaterizedView|null {@inheritDoc}
     */
    public function materializedView($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function materializedViews() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     * @param string|Type $params {@inheritDoc}
     *
     * @return Function|null {@inheritDoc}
     */
    public function function_($name, ...$params) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function functions() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     * @param string|Type $params {@inheritDoc}
     *
     * @return Aggregate|null {@inheritDoc}
     */
    public function aggregate($name, ...$params) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function aggregates() { }

}
