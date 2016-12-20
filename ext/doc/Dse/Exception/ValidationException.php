<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Exception;

/**
 * ValidationException is raised on invalid request, before even attempting to
 * execute it.
 * @see Cassandra\Exception\InvalidSyntaxException
 * @see Cassandra\Exception\UnauthorizedException
 * @see Cassandra\Exception\InvalidQueryException
 * @see Cassandra\Exception\ConfigurationException
 * @see Cassandra\Exception\AlreadyExistsException
 * @see Cassandra\Exception\UnpreparedException
 */
class ValidationException extends RuntimeException  {

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
