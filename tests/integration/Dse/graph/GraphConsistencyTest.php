<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * DSE graph consistency integration tests.
 *
 * @requires DSE >= 5.0.0
 */
class GraphConsistencyTest extends DseGraphIntegrationTest {
    /**
     * Number of nodes to utilize for the test
     */
    const NODES = 3;
    /**
     * Graph consistency read query to perform
     */
    const READ_QUERY = "g.V().limit(1);";
    /**
     * Graph consistency write query to perform
     */
    const WRITE_QUERY = "graph.addVertex(label, 'person', 'name', 'michael', 'age', 27);";

    /**
     * @var bool Flag to determine if a node stop has already been requested
     *           True if schema has already been propegated to the other nodes;
     *           false otherwise
     */
    protected $schema_propagated = false;

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Create the proper cluster configuration for the test
        self::$cluster_configuration->add_data_center(self::NODES);
        $this->replication_factor = self::NODES;

        // Call the parent function
        parent::setUp();
    }

    /**
     * @inheritdoc
     */
    public function tearDown() {
        // Call the parent class
        parent::tearDown();

        // Restart all stopped nodes
        $stopped_nodes = self::$ccm->cluster_status()["down"];
        foreach ($stopped_nodes as $stopped_node) {
            if (self::$configuration->verbose) {
                echo "Restarting Node in {$this->toString()}: "
                    . $stopped_node . PHP_EOL;
            }
            self::$ccm->start_node($stopped_node);
        }
    }

    /**
     * Execute a graph read query with the specified read consistency
     *
     * @param int $consistency Read consistency to apply
     * @return Dse\Graph\ResultSet Graph result from the executed query
     */
    private function execute_read($consistency) {
        $options = array(
            "graph_name" => $this->table,
            "read_consistency" => $consistency
        );
        return $this->session->executeGraph(self::READ_QUERY, $options);
    }

    /**
     * Execute a graph write query with the specified write consistency
     *
     * @param $consistency Write consistency to apply
     * @return Dse\Graph\ResultSet Graph result from the executed query
     */
    private function execute_write($consistency) {
        $options = array(
            "graph_name" => $this->table,
            "write_consistency" => $consistency
        );
        return $this->session->executeGraph(self::WRITE_QUERY, $options);
    }

    /**
     * Helper method to stop a node during the graph consistency test to ensure
     * schema operations have been propagated through the cluster before a node
     * is stopped
     *
     * @param int $node Node to be stopped
     */
    private function stop_node($node) {
        if (!$this->schema_propagated) {
            if (self::$configuration->verbose) {
                echo "Performing graph query to propagate schema across the cluster" . PHP_EOL;
            }
            $this->execute_read(Dse::CONSISTENCY_ONE);
            $this->schema_propagated = true;
        }

        // Stop the requested node
        self::$ccm->stop_node($node);
        sleep(10);
    }

    /**
     * Validate the write query using the result set provided
     *
     * @param Dse\Graph\ResultSet $result_set Result set to validate
     */
    private function validate_result_set($result_set) {
        // Get the vertex from the result set
        $this->assertCount(1, $result_set);
        $result = $result_set[0];
        $this->assertTrue($result->isObject());
        $vertex = $result->asVertex();

        // Validate the properties on the vertex
        $this->assertEquals("person", $vertex->label());
        $this->assertCount(2, $vertex->properties());

        // Validate the name of the person property
        $property = $vertex->property("name")->value();
        $this->assertCount(1, $property);
        $property = $property[0];
        $this->assertTrue($property->isObject());
        $this->assertCount(2, $property);
        foreach ($property as $member) {
            if ($member->isValue()) {
                continue;
            }
        }
        $this->assertTrue($member->isValue());
        $value = $member->asString();
        $this->assertEquals("michael", $value);

        // Validate the age of the person property
        $property = $vertex->property("age")->value();
        $this->assertCount(1, $property);
        $property = $property[0];
        $this->assertTrue($property->isObject());
        $this->assertCount(2, $property);
        foreach ($property as $member) {
            if ($member->isValue()) {
                continue;
            }
        }
        $this->assertTrue($member->isValue());
        $value = $member->asInt();
        $this->assertEquals(27, $value);
    }

    /**
     * Perform a read graph query against a 3 node cluster
     *
     * This test will execute a read graph query against an established graph
     * with all appropriate read consistencies when all node available in the
     * cluster.
     *
     * Consistency levels options applied:
     *   - CONSISTENCY_ONE
     *   - CONSISTENCY_TWO
     *   - CONSISTENCY_THREE
     *   - CONSISTENCY_ALL
     *   - CONSISTENCY_QUORUM
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph read will succeed for all consistency levels
     *                  applied
     *
     * @test
     */
    public function testRead() {
        $this->execute_read(Dse::CONSISTENCY_ONE);
        $this->execute_read(Dse::CONSISTENCY_TWO);
        $this->execute_read(Dse::CONSISTENCY_THREE);
        $this->execute_read(Dse::CONSISTENCY_ALL);
        $this->execute_read(Dse::CONSISTENCY_QUORUM);
    }

    /**
     * Perform a read graph query against a 3 node cluster with 1 node down
     *
     * This test will execute a read graph query against an established graph
     * with all appropriate read consistencies when one node is down in a three
     * node cluster.
     *
     * Consistency levels options applied:
     *   - CONSISTENCY_ONE
     *   - CONSISTENCY_TWO
     *   - CONSISTENCY_QUORUM
     *   Expected to fail:
     *     - CONSISTENCY_ALL
     *     - CONSISTENCY_THREE
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph read will succeed for all consistency levels
     *                  applied and will expect failure to occur for ALL and
     *                  THREE consistency levels
     *
     * @test
     */
    public function testReadOneNodeDown() {
        // Stop the 1st node and run the passing read queries
        $this->stop_node(1);
        $this->execute_read(Dse::CONSISTENCY_ONE);
        $this->execute_read(Dse::CONSISTENCY_TWO);
        $this->execute_read(Dse::CONSISTENCY_QUORUM);

        // Execute the expected failing queries and assert exception
        try {
            $this->execute_read(Dse::CONSISTENCY_ALL);
            $this->fail("Read should have failed for 'ALL' with one node down");
        } catch (Dse\Exception\InvalidQueryException $iqe) {
            $this->assertContains("Operation timed out - received only 2 responses",
                $iqe->getMessage());
        }
        try {
            $this->execute_read(Dse::CONSISTENCY_THREE);
            $this->fail("Read should have failed for 'THREE' with one node down");
        } catch (Dse\Exception\InvalidQueryException $iqe) {
            $this->assertRegExp("(Cannot achieve consistency level|"
                . "Operation timed out - received only 2 responses)",
                $iqe->getMessage());
        }
    }

    /**
     * Perform a write graph query against a 3 node cluster
     *
     * This test will execute a write graph query against an established graph
     * with all appropriate write consistencies when all node available in the
     * cluster.
     *
     * Consistency levels options applied:
     *   - CONSISTENCY_ANY
     *   - CONSISTENCY_ONE
     *   - CONSISTENCY_TWO
     *   - CONSISTENCY_THREE
     *   - CONSISTENCY_ALL
     *   - CONSISTENCY_QUORUM
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph write will succeed for all consistency levels
     *                  applied
     *
     * @test
     */
    public function testWrite() {
        $this->validate_result_set(
            $this->execute_write(Dse::CONSISTENCY_ONE));
        $this->validate_result_set(
            $this->execute_write(Dse::CONSISTENCY_TWO));
        $this->validate_result_set(
            $this->execute_write(Dse::CONSISTENCY_THREE));
        $this->validate_result_set(
            $this->execute_write(Dse::CONSISTENCY_ALL));
        $this->validate_result_set(
            $this->execute_write(Dse::CONSISTENCY_QUORUM));
    }

    /**
     * Perform a write graph query against a 3 node cluster with 1 node down
     *
     * This test will execute a write graph query against an established graph
     * with all appropriate write consistencies when one node is down in a
     * three node cluster.
     *
     * Consistency levels options applied:
     *   - CONSISTENCY_ANY
     *   - CONSISTENCY_ONE
     *   - CONSISTENCY_TWO
     *   - CONSISTENCY_QUORUM
     *   Expected to fail:
     *   - CONSISTENCY_ALL
     *   - CONSISTENCY_THREE
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph write will succeed for all consistency levels
     *                  applied and will expect failure to occur for ALL and
     *                  THREE consistency levels
     *
     * @test
     */
    public function testWriteOneNodeDown() {
        // Stop the 1st node and run the passing write queries
        $this->stop_node(1);
        $this->execute_write(Dse::CONSISTENCY_ONE);
        $this->execute_write(Dse::CONSISTENCY_TWO);
        $this->execute_write(Dse::CONSISTENCY_QUORUM);

        // Execute the expected failing queries and assert exception
        try {
            $this->execute_write(Dse::CONSISTENCY_ALL);
            $this->fail("Write should have failed for 'ALL' with one node down");
        } catch (Dse\Exception\InvalidQueryException $iqe) {
            $this->assertContains("Operation timed out - received only 2 responses",
                $iqe->getMessage());
        }
        try {
            $this->execute_write(Dse::CONSISTENCY_THREE);
            $this->fail("Write should have failed for 'THREE' with one node down");
        } catch (Dse\Exception\InvalidQueryException $iqe) {
            $this->assertContains("Operation timed out - received only 2 responses",
                $iqe->getMessage());
        }
    }
}
