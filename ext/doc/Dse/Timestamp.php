<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `timestamp` datatype
 */
final class Timestamp implements Value {

    /**
     * Creates a new timestamp from either unix timestamp and microseconds or
     * from the current time by default.
     * @param mixed $seconds
     * @param mixed $microseconds
     */
    public function __construct($seconds, $microseconds) { }

    /**
     * The type of this timestamp.
     * @return Type
     */
    public function type() { }

    /**
     * Unix timestamp.
     *
     * @see time
     * @return int seconds
     */
    public function time() { }

    /**
     * Microtime from this timestamp
     *
     *
     * @see microtime
     * @param bool $get_as_float Whether to get this value as float
     * @return float|string Float or string representation
     */
    public function microtime($get_as_float) { }

    /**
     * Converts current timestamp to PHP DateTime.
     * @return \DateTime PHP representation
     */
    public function toDateTime() { }

    /**
     * Returns a string representation of this timestamp.
     * @return string timestamp
     */
    public function __toString() { }

}
