<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Exception;

/**
 * TimeoutException is generally raised when a future did not resolve
 * within a given time interval.
 */
class TimeoutException extends RuntimeException  {

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
