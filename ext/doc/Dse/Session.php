<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
abstract class Session implements Cassandra\Session {

    /**
     * @param mixed $statement
     * @param array $options
     * @return mixed
     */
    public abstract function executeGraph($statement, $options);

    /**
     * @param mixed $statement
     * @param array $options
     * @return mixed
     */
    public abstract function executeGraphAsync($statement, $options);

    /**
     * @param Cassandra\Statement $statement
     * @param Cassandra\ExecutionOptions $options
     * @return mixed
     */
    public abstract function execute($statement, $options);

    /**
     * @param Cassandra\Statement $statement
     * @param Cassandra\ExecutionOptions $options
     * @return mixed
     */
    public abstract function executeAsync($statement, $options);

    /**
     * @param mixed $cql
     * @param Cassandra\ExecutionOptions $options
     * @return mixed
     */
    public abstract function prepare($cql, $options);

    /**
     * @param mixed $cql
     * @param Cassandra\ExecutionOptions $options
     * @return mixed
     */
    public abstract function prepareAsync($cql, $options);

    /**
     * @param mixed $timeout
     * @return mixed
     */
    public abstract function close($timeout);

    /**
     * @return mixed
     */
    public abstract function closeAsync();

    /**
     * @return mixed
     */
    public abstract function schema();

}
