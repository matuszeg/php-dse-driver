<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `date` type.
 */
final class Date implements Value {

    /**
     * Creates a new Date object
     * @param int $seconds Absolute seconds from epoch (1970, 1, 1), can be negative, defaults to current time.
     */
    public function __construct($seconds) { }

    /**
     * The type of this date.
     * @return Type
     */
    public function type() { }

    /**
     * @return int Absolute seconds from epoch (1970, 1, 1), can be negative
     */
    public function seconds() { }

    /**
     * Converts current date to PHP DateTime.
     * @param Time $time An optional Time object that is added to the DateTime object.
     * @return \DateTime PHP representation
     */
    public function toDateTime($time) { }

    /**
     * Creates a new Date object from a \DateTime object.
     * @param \DateTime $datetime A \DateTime object to convert.
     * @return \DateTime PHP representation
     */
    public static function fromDateTime($datetime) { }

    /**
     * @return string this date in string format: Cassandra\Date(seconds=$seconds)
     */
    public function __toString() { }

}
