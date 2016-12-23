<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Behat\Behat\Context\Context;

/**
 * Defines the context for the DSE PHP driver default features
 */
class FeatureContext implements Context {
    // Ensure the common context traits are utilized
    use CommonTraits;

    /**
     * Initializes context
     *
     * @param Cassandra\Version $cassandra_version (Optional) Cassandra version
     *                                                        to use
     * @param DSE\Version $dse_version (Optional) DSE version to use
     */
    public function __construct($cassandra_version = null, $dse_version = null) {}
}