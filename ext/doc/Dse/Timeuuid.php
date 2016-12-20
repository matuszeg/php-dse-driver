<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `timeuuid` datatype
 */
final class Timeuuid implements Value, UuidInterface {

    /**
     * Creates a timeuuid from a given timestamp or current time.
     * @param int $timestamp Unix timestamp
     */
    public function __construct($timestamp) { }

    /**
     * Returns this timeuuid as string.
     * @return string timeuuid
     */
    public function __toString() { }

    /**
     * The type of this timeuuid.
     * @return Type
     */
    public function type() { }

    /**
     * Returns this timeuuid as string.
     * @return string timeuuid
     */
    public function uuid() { }

    /**
     * Returns the version of this timeuuid.
     * @return int version of this timeuuid
     */
    public function version() { }

    /**
     * Unix timestamp.
     *
     * @see time
     * @return int seconds
     */
    public function time() { }

    /**
     * Converts current timeuuid to PHP DateTime.
     * @return \DateTime PHP representation
     */
    public function toDateTime() { }

}
