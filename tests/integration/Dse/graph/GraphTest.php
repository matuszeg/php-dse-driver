<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * DSE graph integration tests.
 *
 * @requires DSE >= 5.0.0
 */
class GraphTest extends DseGraphIntegrationTest {
    /**
     * Graph sleep format query
     */
    const SLEEP_FORMAT = "java.util.concurrent.TimeUnit.MILLISECONDS.sleep(%dL);";

    /**
     * Perform simple graph statement execution - Check for existing graph
     *
     * This test will create a graph and execute a graph statement to determine
     * if that graph exists using the graph result set to parse the
     * information. This will also test the use of single named parameters
     * using the DSE graph object.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph will be created and existence will be verified
     *
     * @test
     */
    public function testGraphExists() {
        $query = "system.graph(name).exists();";

        // Create and execute the graph statement to see if the graph exists
        $options = array(
            "arguments" => array(
                "name" => $this->table
            )
        );
        $result_set = $this->session->executeGraph($query, $options);

        // Validate the result set
        $this->assertCount(1, $result_set);
        $result = $result_set[0];
        $this->assertTrue($result->isBool());
        $this->assertTrue($result->value());
    }

    /**
     * Perform simple graph statement execution - Server Error
     *
     * This test will attempt to drop a non-existing graph that will result in
     * a server side error.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Server side error and exception will be validated
     *
     * @test
     * @expectedException \Dse\Exception\InvalidQueryException
     * @expectedExceptionMessage Graph does_not_exist does not exist
     */
    public function testServerError() {
        $this->session->executeGraph("system.graph('does_not_exist').drop()");
    }

    /**
     * Perform graph statement execution - Multiple named parameters
     *
     * This test will create a graph statement that uses multiple named
     * parameters and validates the assignment using the graph result set to
     * parse the information.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Named parameters will be assigned and validated (textual)
     *
     * @test
     */
    public function testMultipleNamedParameters() {
        // Build the query
        $query = "["
            . "array_value,"
            . "big_integer_value,"
            . "boolean_value,"
            . "double_value,"
            . "integer_value,"
            . "line_string_value,"
            . "null_value,"
            . "object_value,"
            . "point_value,"
            . "polygon_value,"
            . "string_value"
            . "]";

        //TODO: Create a data provider for this test (will be cleaner)
        // Create the graph arguments/values (use a good set of data types)
        $line_string = new Dse\LineString(
            new Dse\Point(0, 0),
            new Dse\Point(1, 1)
        );
        $point = new Dse\Point(0, 0);
        $polygon = new Dse\Polygon(
            new Dse\LineString(
                new Dse\Point(1.0, 3.0),
                new Dse\Point(3.0, 1.0),
                new Dse\Point(3.0, 6.0),
                new Dse\Point(1.0, 3.0)
            )
        );
        $array_value = array(
            new Dse\Bigint(PHP_INT_MAX),
            true,
            3.1415926535,
            2147483647,
            $line_string,
            null,
            $point,
            $polygon,
            "DataStax",
        );
        $object_value = array(
            "big_integer_value" => $array_value[0],
            "boolean_value" => $array_value[1],
            "double_value" => $array_value[2],
            "integer_value" => $array_value[3],
            "line_string_value" => $array_value[4],
            "null_value" => $array_value[5],
            "point_value" => $array_value[6],
            "polygon_value" => $array_value[7],
            "string_value" => $array_value[8]
        );
        $arguments = array(
            "array_value" => $array_value,
            "big_integer_value" => $array_value[0],
            "boolean_value" => $array_value[1],
            "double_value" => $array_value[2],
            "integer_value" => $array_value[3],
            "line_string_value" => $array_value[4],
            "null_value" => $array_value[5],
            "object_value" => $object_value,
            "point_value" => $array_value[6],
            "polygon_value" => $array_value[7],
            "string_value" => $array_value[8]
        );

        // Create and execute the graph statement of arguments/values
        $options = array(
            "arguments" => $arguments
        );
        $result_set = $this->session->executeGraph($query, $options);

        //Validate the result set
        $this->assertCount(11, $result_set);
        $array = $result_set[0];
        $this->assertTrue($array->isArray());
        $this->assertCount(9, $array);
        foreach ($array->value() as $index => $value) {
            //TODO: Use data provider to check type as well
            $value = $value->value();
            $expected = $array_value[$index];
            if ($expected instanceof Dse\Bigint) {
                $expected = $expected->value();
                $value = (string) $value;
            }
            $this->assertEquals($expected, $value);
        }
        $big_integer = $result_set[1];
        $this->assertTrue($big_integer->isValue());
        $this->assertTrue($big_integer->isNumber());
        $this->assertEquals($array_value[0]->value(),
            (string) $big_integer->value());
        $boolean = $result_set[2];
        $this->assertTrue($boolean->isValue());
        $this->assertTrue($boolean->isBool());
        $this->assertEquals($array_value[1], $boolean->value());
        $double = $result_set[3];
        $this->assertTrue($double->isValue());
        $this->assertTrue($double->isNumber());
        $this->assertEquals($array_value[2], $double->value());
        $integer = $result_set[4];
        $this->assertTrue($integer->isValue());
        $this->assertTrue($integer->isNumber());
        $this->assertEquals($array_value[3], $integer->value());
        $line_string = $result_set[5];
        $this->assertTrue($line_string->isValue());
        $this->assertTrue($line_string->isString());
        $this->assertEquals($array_value[4], $line_string->asLineString());
        $null = $result_set[6];
        $this->assertTrue($null->isNUll());
        $this->assertNull($null->value());
        $this->assertEquals($array_value[5], $null->value());
        $object = $result_set[7];
        $this->assertTrue($object->isObject());
        $this->assertCount(9, $object);
        $index = 0;
        foreach ($object->value() as $key => $value) {
            //TODO: Use data provider to check type as well
            $value = $value->value();
            $expected = $object_value[$key];
            if ($expected instanceof Dse\Bigint) {
                $expected = $expected->value();
                $value = (string) $value;
            }
            $this->assertEquals(array_keys($object_value)[$index], $key);
            $this->assertEquals($expected, $value);
            ++$index;
        }
        $point = $result_set[8];
        $this->assertTrue($point->isValue());
        $this->assertTrue($point->isString());
        $this->assertEquals($array_value[6], $point->asPoint());
        $polygon = $result_set[9];
        $this->assertTrue($polygon->isValue());
        $this->assertTrue($polygon->isString());
        $this->assertEquals($array_value[7], $polygon->asPolygon());
        $string = $result_set[10];
        $this->assertTrue($string->isValue());
        $this->assertTrue($string->isString());
        $this->assertEquals($array_value[8], $string->value());
    }

    /**
     * Perform graph statement execution with a specified timestamp
     *
     * This test will create a graph, populate that graph with the classic
     * graph structure example and execute a graph statement using a
     * specified timestamp to retrieve and validate the timestamp the graph
     * result set.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph timestamp set will be validated
     *
     * @test
     */
    public function testTimestamp() {
        $query = "graph.addVertex(label, 'person', 'name', 'michael', 'age', 27);";
        $timestamp = new Dse\Bigint(1270110600000);

        // Create and execute the graph statement with specified timestamp
        $options = array(
            "graph_name" => $this->table,
            "timestamp" => (string) $timestamp // Use string as $timestamp might be to big for zend_long
        );
        $result_set = $this->session->executeGraph($query, $options);


        // Validate the result (obtain the community ID)
        $result = $result_set[0];
        $this->assertTrue($result->isObject());
        $id = $result->value()["id"];
        $this->assertTrue($id->isObject());
        $community_id = $id->value()["community_id"];
        $this->assertTrue($community_id->isValue());
        $this->assertTrue($community_id->isNumber());
        $community_id = $community_id->value();

        // Validate the timestamp (+1 from inserted value)
        $query = "SELECT WRITETIME(\"~~vertex_exists\") "
            . "FROM {$this->table}.person_p "
            . "WHERE community_id={$community_id}";
        $rows = $this->session->execute($query);
        $expected_timestamp = $timestamp->add(new Dse\Bigint(1));
        $this->assertEquals($expected_timestamp, $rows[0]["writetime(~~vertex_exists)"]);
    }

    /**
     * Perform graph statement execution to ensure client timeouts are
     * respected
     *
     * This test will create a graph statement that uses request timeout
     * options and validates client side timeouts.
     *
     * (1) By nature of the implementation of request timeout, the core driver
     *     per request timeout is also tested in this test case.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph request client timeouts will be honored
     *
     * @test
     * @expectedException Dse\Exception\TimeoutException
     * @expectedExceptionMessage Request timed out
     */
    public function testClientRequestTimeout() {
        $query = sprintf(self::SLEEP_FORMAT, 5000);

        // Execute a graph statement that executes longer than a client request
        $options = array(
            "request_timeout" => 0.5 // 500ms
        );
        $this->session->executeGraph($query, $options);
    }

    /**
     * Perform graph statement execution to ensure server timeouts are
     * respected
     *
     * This test will create a graph statement that uses the default and
     * specified request timeout options to validate server side timeouts
     * (e.g. driver timeout is infinite).
     *
     * (1) By nature of the implementation of request timeout, the core driver
     *     per request timeout is also tested in this test case.
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Graph request server timeouts will be honored
     *
     * @test
     * @expectedException Dse\Exception\InvalidQueryException
     * @expectedExceptionMessageRegExp |.*exceeded.*1243 ms.*|
     */
    public function testServerRequestTimeout() {
        // Create a graph with a server side timeout of 1234 ms
        $this->drop_graph();
        $this->create_graph(null, null, "PT1.243S");

        // Execute a graph statement that executes longer than a client request
        $query = sprintf(self::SLEEP_FORMAT, 5000);
        $options = array(
            "graph_name" => $this->table
        );
        $this->session->executeGraph($query, $options);
    }
}