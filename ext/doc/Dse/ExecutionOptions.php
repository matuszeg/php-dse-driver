<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Request execution options.
 *
 * @deprecated Use an array of options instead of creating an instance of this class.
 *
 * @see Session::execute()
 * @see Session::executeAsync()
 * @see Session::prepare()
 * @see Session::prepareAsync()
 */
final class ExecutionOptions {

    /**
     * Creates a new options object for execution.
     *
     * @param array $options various execution options
     *
     * @throws Exception\InvalidArgumentException
     *
     * @see Session::execute() for valid execution options
     */
    public function __construct($options) { }

    /**
     * @param mixed $name
     * @return mixed
     */
    public function __get($name) { }

}
