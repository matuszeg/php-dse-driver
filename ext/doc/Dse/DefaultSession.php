<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
final class DefaultSession extends Session  {

    /**
     * @param Cassandra\Statement $statement 
     * @param Cassandra\ExecutionOptions $options 
     * @return mixed 
     */
    public function execute($statement, $options) { }

    /**
     * @param Cassandra\Statement $statement 
     * @param Cassandra\ExecutionOptions $options 
     * @return mixed 
     */
    public function executeAsync($statement, $options) { }

    /**
     * @param mixed $cql 
     * @param Cassandra\ExecutionOptions $options 
     * @return mixed 
     */
    public function prepare($cql, $options) { }

    /**
     * @param mixed $cql 
     * @param Cassandra\ExecutionOptions $options 
     * @return mixed 
     */
    public function prepareAsync($cql, $options) { }

    /**
     * @param mixed $timeout 
     * @return mixed 
     */
    public function close($timeout) { }

    /**
     * @return mixed 
     */
    public function closeAsync() { }

    /**
     * @return mixed 
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
