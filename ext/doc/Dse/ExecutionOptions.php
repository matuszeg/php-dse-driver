<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Request execution options.
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
     * * array['arguments']          array       An array or positional or named arguments
     * * array['consistency']        int         One of Dse::CONSISTENCY_*
     * * array['timeout']            int|null    A number of seconds or null
     * * array['page_size']          int         A number of rows to include in result for paging
     * * array['paging_state_token'] string      A string token use to resume from the state of a previous result set
     * * array['retry_policy']       RetryPolicy A retry policy that is used to handle server-side failures for this request
     * * array['serial_consistency'] int         Either Dse::CONSISTENCY_SERIAL or Dse::CONSISTENCY_LOCAL_SERIAL
     * * array['timestamp']          int|string  Either an integer or integer string timestamp that represents the number
     *                                           of microseconds since the epoch.
     * * array['execute_as']         string      User to execute statement as.
     *
     * @param array $options various execution options
     *
     * @throws Exception\InvalidArgumentException
     */
    public function __construct($options) { }

    /**
     * @param mixed $name
     * @return mixed
     */
    public function __get($name) { }

}
