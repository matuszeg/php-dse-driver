<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Exception;

/**
 * WriteTimeoutException is raised when a coordinator failed to receive acks
 * from the required number of replica nodes in time during a write.
 * @see https://github.com/apache/cassandra/blob/cassandra-2.1/doc/native_protocol_v1.spec#L683-L708 Description of WriteTimeout error in the native protocol spec
 */
class WriteTimeoutException extends ExecutionException  {

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
