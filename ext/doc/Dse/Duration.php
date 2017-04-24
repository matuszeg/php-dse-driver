<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `duration` datatype
 */
final class Duration implements Value {

    /**
     * @param long|double|string|Bigint $months Months attribute of the duration.
     * @param long|double|string|Bigint $days Days attribute of the duration.
     * @param long|double|string|Bigint $nanos Nanos attribute of the duration.
     */
    public function __construct($months, $days, $nanos) { }

    /**
     * The type of represented by the value.
     *
     * @return Type the type for Duration
     */
    public function type() { }

    /**
     * @return string the months attribute of this Duration
     */
    public function months() { }

    /**
     * @return string the days attribute of this Duration
     */
    public function days() { }

    /**
     * @return string the nanoseconds attribute of this Duration
     */
    public function nanos() { }

    /**
     * @return string string representation of this Duration; may be used as a literal parameter in CQL queries.
     */
    public function __toString() { }

}
