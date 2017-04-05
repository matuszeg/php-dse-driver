<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the DSE `DateRange` datatype
 */
final class DateRange extends Custom  {

    /**
     * Create a DateRange object
     * @code{.php}
     * <?php
     * use Dse\DateRange;
     * use Dse\DateRange\Bound;
     * use Dse\DateRange\Precision;
     *
     * // DateRange constructed with a single Bound object.
     * $bound = new Bound(Precision::YEAR, 1491252166000);
     * $dr = new DateRange($bound);
     *
     * // A DateRange with lower and upper bounds, using a mix of Bound object and raw values
     * $dr = new DateRange($bound, Precision::DAY, 1491252266000);
     *
     * // Range with no lower bound
     * $dr = new DateRange(Bound::unbounded(), $bound);
     * @endcode
     *
     * @param mixed $bounds A variadic list of args that denote a single-date bound or a lower and upper bound pair
     */
    public function __construct($bounds) { }

    /**
     * {@inheritDoc}
     *
     * @return Type\Custom the custom type for DateRange
     */
    public function type() { }

    /**
     * @return Bound the lower-bound or single-date-bound of this DateRange
     */
    public function lowerBound() { }

    /**
     * @return Bound the upper-bound of this DateRange; `null` if this is a single-date DateRange
     */
    public function upperBound() { }

    /**
     * @return boolean whether or not this DateRange has a single date value
     */
    public function isSingleDate() { }

    /**
     * @return string string representation of this DateRange
     */
    public function __toString() { }

}
