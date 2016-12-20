<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Exception;

/**
 * UnavailableException is raised when a coordinator detected that there aren't
 * enough replica nodes available to fulfill the request.
 *
 * NOTE: Request has not even been forwarded to the replica nodes in this case.
 * @see https://github.com/apache/cassandra/blob/cassandra-2.1/doc/native_protocol_v1.spec#L667-L677 Description of the Unavailable error in the native protocol v1 spec.
 */
class UnavailableException extends ExecutionException  {

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
