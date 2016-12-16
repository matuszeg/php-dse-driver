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
     * @return mixed
     */
    public function execute($statement, $options) { }

    /**
     * @return mixed
     */
    public function executeAsync($statement, $options) { }

    /**
     * @return mixed
     */
    public function prepare($cql, $options) { }

    /**
     * @return mixed
     */
    public function prepareAsync($cql, $options) { }

    /**
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
     * @return mixed
     */
    public function executeGraph($statement, $options) { }

    /**
     * @return mixed
     */
    public function executeGraphAsync($statement, $options) { }

}
