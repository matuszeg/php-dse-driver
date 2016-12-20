<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `time` type.
 */
final class Time implements Value {

    /**
     * Creates a new Time object
     * @param int|string $nanoseconds Number of nanoseconds since last microsecond
     */
    public function __construct($nanoseconds) { }

    /**
     * The type of this date.
     * @return Type
     */
    public function type() { }

    /**
     * @return mixed
     */
    public function seconds() { }

    /**
     * @param DateTime $datetime
     * @return mixed
     */
    public static function fromDateTime($datetime) { }

    /**
     * @return string this date in string format: Cassandra\Time(nanoseconds=$nanoseconds)
     */
    public function __toString() { }

}
