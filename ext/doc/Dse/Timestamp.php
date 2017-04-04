<?php

/*
  Copyright (c) 2017 DataStax, Inc.
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
     *
     * @param int $seconds The number of seconds
     * @param int $microseconds The number of microseconds
     */
    public function __construct($seconds, $microseconds) { }

    /**
     * The type of this timestamp.
     *
     * @return Type
     */
    public function type() { }

    /**
     * Unix timestamp.
     *
     * @return int seconds
     *
     * @see time
     */
    public function time() { }

    /**
     * Microtime from this timestamp
     *
     * @param bool $get_as_float Whether to get this value as float
     *
     * @return float|string Float or string representation
     *
     * @see microtime
     */
    public function microtime($get_as_float) { }

    /**
     * Converts current timestamp to PHP DateTime.
     *
     * @return \DateTime PHP representation
     */
    public function toDateTime() { }

    /**
     * Returns a string representation of this timestamp.
     *
     * @return string timestamp
     */
    public function __toString() { }

}
