<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * Base class to provide common integration test functionality for DSE graph
 * testing.
 *
 * @requires DSE >= 5.0.0
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
     * Drop graph query
     */
    const DROP = "system.graph(name).drop();";
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
     * @var bool Flag to determine if the classic graph structure examples should
     *           be created.
     */
    protected $create_graph = true;
    /**
     * @var bool Flag to determine if the classic graph structure examples should
     *           be populated.
     */
    protected $populate_graph = true;

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Enable graph workload
        self::$cluster_configuration->add_workload(\CCM\Workload::GRAPH);

        // Call the parent function
        parent::setUp();

        // Create the classic graph structure
        if ($this->create_graph) {
            $this->create_graph();
            if ($this->populate_graph) {
                $this->populate_classic_graph();
            }
        }
    }

    /**
     * @inheritdoc
     */
    public function tearDown() {
        // Drop the default graph
        if (!is_null($this->session)) {
            try {
                $this->drop_graph();
            } catch (\Exception $e) {
                ; // no-op
            }
        }

        // Call the parent function
        parent::tearDown();
    }

    /**
     * Create a graph for use with the DSE graph integration tests
     *
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
     * DSE data types (graph)
     *
     * @see DseIntegrationTest::geometry_data_types()
     *
     * @param bool $primary_keys (Optional) True if data types will be used as
     *                                      a primary key; false otherwise
     *                                      (default: false)
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [0] => (Dse\Type|string) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function data_types($primary_keys = false) {
        // Create the array representing all the DSE graph data types
        $data_types = array(
            // Duration data type
            array(
                "duration",
                array(
                    "5 s",
                    "5 seconds",
                    "1 minute",
                    "P1DT1H4M1S",
                    "P2DT3H4M5S"
                ),
                array(
                    "PT5S",
                    "PT5S",
                    "PT1M",
                    "PT25H4M1S",
                    "PT51H4M5S"
                )
            )
        );
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version) {
            if ($version->compare("5.1.0") >= 0) {
                // Date
                $data_types[] = array(
                    "date",
                    array(
                        date("Y-m-d"),
                        date("Y-m-d", strtotime("2010-04-01"))
                    )
                );
                // Time
                $data_types[] = array(
                    "time",
                    array(
                        "11:33:45.556",
                        "11:33:45.556000037",
                        "11:33:45"
                    )
                );
                // Time (with expected results)
                $data_types[] = array(
                    "time",
                    array(
                        "00:00:00"
                    ),
                    array(
                        "00:00"
                    )
                );
            }
        }

        // Add the Cassandra data types
        foreach (parent::data_types($primary_keys) as $data_type) {
            // Ensure the data types that aren't applicable for graph are removed
            // https://docs.datastax.com/en/latest-dse/datastax_enterprise/graph/reference/refDSEGraphDataTypes.html
            $type = $data_type[0];
            if ($type !== Dse\Type::ascii() &&
                $type !== Dse\Type::date() &&
                $type !== Dse\Type::dateRange() &&
                $type !== Dse\Type::duration() && // Duration is already defined
                $type !== Dse\Type::time() &&
                $type !== Dse\Type::timeuuid() &&
                $type !== Dse\Type::tinyint() &&
                $type !== Dse\Type::varchar() &&
                !$this->is_composite_data_type($type)) {
                $data_types[] = $data_type;
            }
        }

        // Add the geometry data types
        return $data_types;
    }

    /**
     * Drop a graph for use with the DSE graph integration tests
     *
     * @param string|null $name (Optional) Name of the graph to create
     *                                     (default: test name)
     * @param string $duration (Optional) Maximum duration to wait for the
     *                                    traversal to evaluate (default: 30s)
     */
    protected function drop_graph($name = null, $duration = "PT30S") {
        // Determine if the default name and/or replication strategy is required
        if (is_null($name)) {
            $name = $this->table;
        }

        // Create the graph using the required arguments
        $options = array(
            "arguments" => array(
                "name" => $name,
                "duration" => $duration
            )
        );
        $this->session->executeGraph(self::DROP, $options);
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