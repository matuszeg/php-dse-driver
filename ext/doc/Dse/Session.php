<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A session is used to prepare and execute statements.
 *
 * @see Cluster::connect()
 */
interface Session {

    /**
     * Executes a given statement and returns a result.
     *
     * @throws Exception
     *
     * @param Cassandra\Statement $statement statement to be executed
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return Rows execution result
     */
    public function execute($statement, $options);

    /**
     * Executes a given statement and returns a future result.
     *
     * Note that this method ignores timeout specified in the ExecutionOptions,
     * you can provide one to Future::get() instead.
     *
     * @param Cassandra\Statement $statement statement to be executed
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return Future future result
     */
    public function executeAsync($statement, $options);

    /**
     * Creates a prepared statement from a given CQL string.
     *
     * Note that this method only uses the ExecutionOptions::$timeout option,
     * all other options will be ignored.
     *
     * @throws Exception
     *
     * @param string $cql CQL statement string
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return PreparedStatement prepared statement
     */
    public function prepare($cql, $options);

    /**
     * Asynchronously prepares a statement and returns a future prepared statement.
     *
     * Note that all options passed to this method will be ignored.
     *
     * @param string $cql CQL string to be prepared
     * @param Cassandra\ExecutionOptions $options preparation options
     * @return Future statement
     */
    public function prepareAsync($cql, $options);

    /**
     * Closes current session and all of its connections.
     * @param float|null $timeout Timeout to wait for closure in seconds
     * @return void
     */
    public function close($timeout);

    /**
     * Asynchronously closes current session once all pending requests have finished.
     * @return Future future
     */
    public function closeAsync();

    /**
     * Returns current schema.
     *
     * NOTE: the returned Schema instance will not be updated as the actual
     *       schema changes, instead an updated instance should be requested by
     *       calling Session::schema() again.
     * @return Schema current schema.
     */
    public function schema();

}
