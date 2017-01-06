<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
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
    private $session;

    /**
     * Initializes context
     *
     * @param DSE\Version $dse_version DSE version to use
     */
    public function __construct($dse_version) { }

    /**
     * Create a graph with a specific name.
     *
     * @param string $graphName
     * @Given /^an existing graph called "(.*?)"$/
     */
    public function given_an_existing_graph($graphName) {
        if (!isset($this->session)) {
            $this->session = Dse::cluster()->build()->connect();
        }
        $replicationConfig = "{ 'class' : 'SimpleStrategy', 'replication_factor' : 1 }";
        $this->session->executeGraph("system.graph(\"$graphName\").option('graph.replication_config').set(\"$replicationConfig\").ifNotExists().create()");
        $this->session->executeGraph("schema.config().option('graph.schema_mode').set(com.datastax.bdp.graph.api.model.Schema.Mode.Production)",
            array("graph_name" => $graphName));
        $this->session->executeGraph("schema.config().option('graph.allow_scan').set('true')",
            array("graph_name" => $graphName));
    }

    /**
     * Add schema to a given graph.
     *
     * @param string $graphName
     * @param string $schema
     * @Given /^the following graph schema for "(.*?)":$/
     */
    public function given_the_following_graph_schema($graphName, $schema) {
        if (!isset($this->session)) {
            $this->session = Dse::cluster()->build()->connect();
        }
        $this->session->executeGraph((string)$schema, array("graph_name" => $graphName, "write_consistency" => Dse::CONSISTENCY_ONE));
    }

    /**
     * Create a graph with a specific name and schema.
     *
     * @param string $graphName
     * @param string $schema
     * @Given /^an existing graph called "(.*?)" with schema:$/
     */
    public function given_an_existing_graph_with_schema($graphName, $schema) {
        $this->given_an_existing_graph($graphName);
        $this->given_the_following_graph_schema($graphName, $schema);
    }

    /**
     * Create a DSE cluster with a graph or graph analytics workload.
     *
     * @param string $workload Workload type
     * @Given /^a running DSE cluster with (graph|graph and spark)$/
     */
    public function a_running_cluster_with_workload($workload) {
        if ($workload === "graph") {
            $configuration = array("workloads" => array(CCM\Workload::GRAPH));
        } else if($workload === "graph and spark") {
            $configuration = array("workloads" => array(CCM\Workload::GRAPH, CCM\Workload::SPARK));
        }

        $this->a_running_cluster_with_nodes("DSE", 1, $configuration);
    }

}
