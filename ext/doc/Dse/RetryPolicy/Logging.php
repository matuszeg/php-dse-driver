<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\RetryPolicy;

/**
 * A retry policy that logs the decisions of its child policy.
 */
final class Logging implements RetryPolicy {

    /**
     * Creates a new Logging retry policy.
     *
     *                                           Cassandra\Logging
     * @param Cassandra\RetryPolicy $childPolicy Any retry policy other than
     */
    public function __construct($childPolicy) { }

}