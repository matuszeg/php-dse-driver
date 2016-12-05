<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * Base class to provide common integration test functionality for DSE graph
 * testing.
 *
 * @requires DSE >= 5.1.0
 */
abstract class DseGraphIntegrationTest extends DseIntegrationTest {
    /**
     * Create graph query
     */
    const CREATE = "system.graph(name).option('graph.replication_config').set(replication)"
        . ".option('graph.system_replication_config').set(replication)"
        . ".option('graph.traversal_sources.g.evaluation_timeout').set(duration)"
        . ".ifNotExists().create();";
    /**
     * Graph query to enable table scans
     */
    const ALLOW_SCANS = "schema.config().option('graph.allow_scan').set('true')";
    /**
     * Graph query to enable strict/production mode
     */
    const ENABLE_STRICT = "schema.config().option('graph.schema_mode').set('production')";
    /**
     * Graph schema used by classic graph structure examples in TinkerPop
     * documentation
     *
     * @see http://tinkerpop.apache.org/docs/3.1.0-incubating/#intro
     */
    const SCHEMA = "schema.propertyKey('name').Text().ifNotExists().create();"
        . "schema.propertyKey('age').Int().ifNotExists().create();"
        . "schema.propertyKey('lang').Text().ifNotExists().create();"
        . "schema.propertyKey('weight').Float().ifNotExists().create();"
        . "schema.vertexLabel('person').properties('name', 'age').ifNotExists().create();"
        . "schema.vertexLabel('software').properties('name', 'lang').ifNotExists().create();"
        . "schema.edgeLabel('created').properties('weight').connection('person', 'software').ifNotExists().create();"
        . "schema.edgeLabel('created').connection('software', 'software').add();"
        . "schema.edgeLabel('knows').properties('weight').connection('person', 'person').ifNotExists().create();";
    /**
     * Graph data used by classic graph structure examples in TinkerPop
     * documentation
     *
     * @see http://tinkerpop.apache.org/docs/3.1.0-incubating/#intro
     */
    const DATA = "Vertex marko = graph.addVertex(label, 'person', 'name', 'marko', 'age', 29);"
        . "Vertex vadas = graph.addVertex(label, 'person', 'name', 'vadas', 'age', 27);"
        . "Vertex lop = graph.addVertex(label, 'software', 'name', 'lop', 'lang', 'java');"
        . "Vertex josh = graph.addVertex(label, 'person', 'name', 'josh', 'age', 32);"
        . "Vertex ripple = graph.addVertex(label, 'software', 'name', 'ripple', 'lang', 'java');"
        . "Vertex peter = graph.addVertex(label, 'person', 'name', 'peter', 'age', 35);"
        . "marko.addEdge('knows', vadas, 'weight', 0.5f);"
        . "marko.addEdge('knows', josh, 'weight', 1.0f);"
        . "marko.addEdge('created', lop, 'weight', 0.4f);"
        . "josh.addEdge('created', ripple, 'weight', 1.0f);"
        . "josh.addEdge('created', lop, 'weight', 0.4f);"
        . "peter.addEdge('created', lop, 'weight', 0.2f);";

    /**
     * @param string|null $name (Optional) Name of the graph to create
     *                                     (default: test name)
     * @param string|null $replication_strategy (Optional) Replication strategy
     *                                                     to apply to the graph
     *                                                     (default: generated
     *                                                               replication
     *                                                               strategy)
     * @param string $duration (Optional) Maximum duration to wait for the
     *                                    traversal to evaluate (default: 30s)
     */
    protected function create_graph($name = null,
                                    $replication_strategy = null,
                                    $duration = "PT30S") {
        // Determine if the default name and/or replication strategy is required
        if (is_null($name)) {
            $name = $this->table;
        }
        if (is_null($replication_strategy)) {
            $replication_strategy = $this->replication_strategy;
        }

        // Create the graph using the required arguments
        $options = array(
            "arguments" => array(
                "name" => $name,
                "replication" => $replication_strategy,
                "duration" => $duration
            )
        );
        $this->session->executeGraph(self::CREATE, $options);

        // Enable testing functionality for the graph
        $options = array(
            "graph_name" => $name
        );
        $this->session->executeGraph(self::ALLOW_SCANS, $options);
        $this->session->executeGraph(self::ENABLE_STRICT, $options);
    }

    /**
     * Populate the graph with the classic graph structure examples used by the
     * TinkerPop documentation
     *
     * @see http://tinkerpop.apache.org/docs/3.1.0-incubating/#intro
     *
     * @param string|null $name (Optional) Name of the graph to populate
     *                                     (default: test name)
     */
    protected function populate_classic_graph($name = null) {
        // Determine if the default name is required
        if (is_null($name)) {
            $name = $this->table;
        }

        // Initialize the graph and populate the classic graph structure
        $options = array(
            "graph_name" => $name
        );
        $this->session->executeGraph(self::SCHEMA, $options);
        $this->session->executeGraph(self::DATA, $options);
    }
}