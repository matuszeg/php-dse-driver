<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Exception;

/**
 * ExecutionException is raised when something went wrong during request execution.
 * @see TruncateException
 * @see UnavailableException
 * @see ReadTimeoutException
 * @see WriteTimeoutException
 */
class ExecutionException extends RuntimeException  {

    /**
     * @param mixed $message
     * @param mixed $code
     * @param mixed $previous
     */
    public function __construct($message, $code, $previous) { }

    /**
     * @return mixed
     */
    public function __wakeup() { }

    /**
     * @return mixed
     */
    public function __toString() { }

}
