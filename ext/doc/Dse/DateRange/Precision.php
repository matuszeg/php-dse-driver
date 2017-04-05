<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\DateRange;

/**
 * Contains constants for the valid precisions; cannot be instantiated.
 */
abstract class Precision {

    /**
     */
    const UNBOUNDED = 255;

    /**
     */
    const YEAR = 0;

    /**
     */
    const MONTH = 1;

    /**
     */
    const DAY = 2;

    /**
     */
    const HOUR = 3;

    /**
     */
    const MINUTE = 4;

    /**
     */
    const SECOND = 5;

    /**
     */
    const MILLISECOND = 6;

}
