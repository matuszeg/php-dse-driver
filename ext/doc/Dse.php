<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

/**
 * Provides access to builder objects used to configure Dse\Cluster and
 * Cassandra\SSLOptions objects.
 */
class Dse extends Cassandra {

    /**
     * Creates a new cluster builder for constructing a Dse\Cluster object.
     * @return Dse\Cluster\Builder A cluster builder object with default settings
     */
    public static function cluster() { }

    /**
     * Creates a new graph options builder for constructing a Dse\Graph\Options object.
     * @return Dse\Graph\Options A graph options builder with default settings
     */
    public static function graphOptions() { }

    /**
     * Creates a new ssl builder for constructing a Cassandra\SSLOptions object.
     * @return Cassandra\SSLOptions\Builder A SSL options builder with default settings
     */
    public static function ssl() { }

}
