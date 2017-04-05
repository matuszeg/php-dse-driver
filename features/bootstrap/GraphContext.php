<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Behat\Behat\Context\Context;
use Behat\Behat\Hook\Scope\AfterFeatureScope;
use Behat\Behat\Hook\Scope\BeforeFeatureScope;

/**
 * Defines the context for the DSE PHP driver graph features
 */
class GraphContext implements Context {
    // Ensure the common context traits are utilized
    use CommonTraits;

    /**
     * @var Dse\Session A session for executing graph schema setup
     */
    private static $session;

    /**
     * Initializes context
     *
     * @param DSE\Version $dse_version DSE version to use
     */
    public function __construct($dse_version) { }

    /**
     * Handle feature setup for graph context
     *
     * @param BeforeFeatureScope $scope The scope before the feature has
     *                                  started
     * @BeforeFeature
     */
    public static function setup_feature_ads(BeforeFeatureScope $scope) {
        self::$session = self::default_session();
    }

    /**
     * Create a graph with a specific name.
     *
     * @param string $graph_name
     * @Given /^an existing graph called "(.*?)"$/
     */
    public function given_an_existing_graph($graph_name) {
        $replication_config = "{ 'class' : 'SimpleStrategy', 'replication_factor' : 1 }";
        $options = array(
            "graph_name" => $graph_name
        );
        self::$session->executeGraph("system.graph(\"{$graph_name}\").option('graph.replication_config').set(\"{$replication_config}\").ifNotExists().create()");
        self::$session->executeGraph("schema.config().option('graph.schema_mode').set(com.datastax.bdp.graph.api.model.Schema.Mode.Production)",
            $options);
        self::$session->executeGraph("schema.config().option('graph.allow_scan').set('true')",
            $options);
    }

    /**
     * Add schema to a given graph.
     *
     * @param string $graph_name
     * @param string $schema
     * @Given /^the following graph schema for "(.*?)":$/
     */
    public function given_the_following_graph_schema($graph_name, $schema) {
        $options = array(
            "graph_name" => $graph_name,
            "write_consistency" => Dse::CONSISTENCY_ONE
        );
        self::$session->executeGraph((string) $schema, $options);
    }

    /**
     * Create a graph with a specific name and schema.
     *
     * @param string $graph_name
     * @param string $schema
     * @Given /^an existing graph called "(.*?)" with schema:$/
     */
    public function given_an_existing_graph_with_schema($graph_name, $schema) {
        $this->given_an_existing_graph($graph_name);
        $this->given_the_following_graph_schema($graph_name, $schema);
    }

    /**
     * Create a DSE cluster with a graph or graph analytics workload.
     *
     * @param string $workload Workload type
     * @Given /^a running DSE cluster with (graph|graph and spark)$/
     */
    public function a_running_cluster_with_workload($workload) {
        if ($workload === "graph") {
            $configuration = array(
                "workloads" => array(
                    CCM\Workload::GRAPH
                )
            );
        } else if($workload === "graph and spark") {
            $configuration = array(
                "workloads" => array(
                    CCM\Workload::GRAPH,
                    CCM\Workload::SPARK
                )
            );
        }

        $this->a_running_cluster_with_nodes("DSE", 1, $configuration);
    }

}
