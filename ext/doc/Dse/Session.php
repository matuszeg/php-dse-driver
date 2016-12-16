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
     * @return mixed
     */
    public abstract function executeGraph($statement, $options);

    /**
     * @return mixed
     */
    public abstract function executeGraphAsync($statement, $options);

    /**
     * @return mixed
     */
    public abstract function execute($statement, $options);

    /**
     * @return mixed
     */
    public abstract function executeAsync($statement, $options);

    /**
     * @return mixed
     */
    public abstract function prepare($cql, $options);

    /**
     * @return mixed
     */
    public abstract function prepareAsync($cql, $options);

    /**
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
