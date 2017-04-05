<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\DateRange;

/**
 * PHP class that encapsulates one end of a date-range
 */
final class Bound {

    /**
     * @param long $precision desired precision of this Bound; one of the constants from the Precision class
     * @param long|double|string|Bigint|DateTime $timeMs millisecond-precision time
     */
    public function __construct($precision, $timeMs) { }

    /**
     * @return long precision attribute of this Bound
     */
    public function precision() { }

    /**
     * @return string timeMs attribute of this Bound
     */
    public function timeMs() { }

    /**
     * @return Bound a Bound instance that represents "no bound". It is used as the virtual end-point of an open-ended range.
     */
    public static function unbounded() { }

}
