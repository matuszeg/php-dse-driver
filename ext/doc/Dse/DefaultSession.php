<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Actual session implementation.
 *
 * @see Cassandra\Session
 */
final class DefaultSession implements Session {

    /**
     * {@inheritDoc}
     *
     * @throws Exception
     *
     * @param Cassandra\Statement $statement statement to be executed
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return Rows execution result
     */
    public function execute($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param Cassandra\Statement $statement statement to be executed
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return Future future result
     */
    public function executeAsync($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @throws Exception
     *
     * @param string $cql CQL statement string
     * @param Cassandra\ExecutionOptions $options execution options (optional)
     * @return PreparedStatement prepared statement
     */
    public function prepare($cql, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string $cql CQL string to be prepared
     * @param Cassandra\ExecutionOptions $options preparation options
     * @return Future statement
     */
    public function prepareAsync($cql, $options) { }

    /**
     * {@inheritDoc}
     * @param float|null $timeout Timeout to wait for closure in seconds
     * @return void
     */
    public function close($timeout) { }

    /**
     * {@inheritDoc}
     * @return Future future
     */
    public function closeAsync() { }

    /**
     * {@inheritDoc}
     * @return Schema current schema.
     */
    public function schema() { }

    /**
     * @param mixed $statement
     * @param array $options
     * @return mixed
     */
    public function executeGraph($statement, $options) { }

    /**
     * @param mixed $statement
     * @param array $options
     * @return mixed
     */
    public function executeGraphAsync($statement, $options) { }

}
