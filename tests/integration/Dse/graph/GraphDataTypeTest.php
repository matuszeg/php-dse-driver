<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * DSE graph data type integration tests.
 *
 * @requires DSE >= 5.0.0
 */
class GraphDataTypeTest extends DseGraphIntegrationTest {
    /**
     * Schema format for the data types
     */
    const SCHEMA_FORMAT  = "schema.propertyKey(property_name).%s().create();"
        . "schema.vertexLabel(vertex_label).properties(property_name).create();"
        . "schema.vertexLabel(vertex_label).index(property_name + 'Index').secondary().by(property_name).add();";
    /**
     * Insert graph query
     */
    const INSERT = "g.addV(label, vertex_label, property_name, value_field);";
    /**
     * Select graph query
     */
    const SELECT = "g.V().hasLabel(vertex_label).has(property_name, value_field).next();";

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Disable graph population and indicate data provider is in use
        $this->populate_graph = false;
        $this->using_data_provider = true;

        // Call the parent function
        parent::setUp();
    }

    /**
     * Data provider for graph data types
     *
     * @see DseGraphIntegrationTest::data_types()
     *
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [
     *             [0] => (string) Graph schema
     *             [1] => (Dse\Type|string) Data type
     *             [2] => (mixed) Data type value
     *             [3] => (mixed) Expected value (validation)
     *         ]
     *     ]
     * @throws \Exception If the sizes of values and expected values are
     *                    different
     */
    public function data_type_provider() {
        return $this->provider(false);
    }

    /**
     * Generate the CQL data type from the give data type
     *
     * @param Dse\Type|string $type Data type
     * @param bool $for_composite (Optional) True if composite data types
     *                                       should be frozen for use in a
     *                                       nested composite data type; false
     *                                       otherwise (default: false)
     * @return string String representation of the CQL data type
     */
    protected function generate_cql_data_type($type) {
        // Determine if the data type is a geospatial data type
        if (strcasecmp("linestring", $type) == 0) {
            return "Linestring";
        } else if (strcasecmp("point", $type) == 0) {
            return "Point";
        } else if (strcasecmp("polygon", $type) == 0) {
            return "Polygon";
        }

        // Call the main class to determine CQL data type
        return ucfirst(IntegrationTest::generate_cql_data_type($type));
    }

    /**
     * Data provider for graph data types
     *
     * @see DseGraphIntegrationTest::data_types()
     *
     * @param bool $primary_keys True if data types will be used as a primary
     *                           key; false otherwise
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [
     *             [0] => (string) Graph schema
     *             [1] => (Dse\Type|string) Data type
     *             [2] => (mixed) Data type value
     *             [3] => (mixed) Expected value (validation)
     *         ]
     *     ]
     * @throws \Exception If the sizes of values and expected values are
     *                    different
     */
    protected function provider($primary_keys) {
        // Expand data types into a provider
        $provider = array();
        $count = 1;
        foreach ($this->data_types($primary_keys) as $data_types) {
            // Ensure the values and expected values are the same size
            $contains_expected = (count($data_types) == 3);
            if ($contains_expected) {
                $values_count = count($data_types[1]);
                $expected_count = count($data_types[2]);
                if ($values_count != $expected_count) {
                    throw new \Exception("Values [$values_count] != Expected [$expected_count]");
                }
            }

            // Get the data type and graph type
            $type = $data_types[0];
            $graph_type = $this->generate_cql_data_type($type);

            // Iterate over each value and update the provider (flatten array)
            foreach ($data_types[1] as $index => $value) {
                // Create the parameters for the provider
                $parameters = array();
                $parameters[] = sprintf(self::SCHEMA_FORMAT, $graph_type);
                $parameters[] = $type;
                $parameters[] = $value;
                if ($contains_expected) {
                    $parameters[] = $data_types[2][$index];
                } else {
                    $parameters[] = $value;
                }

                // Name the provider and add the parameters
                $provider_name = "{$graph_type}";
                $provider_name_suffix = "#" . $count++;
                if (IntegrationTestFixture::get_instance()->configuration->verbose) {
                    $provider_name_suffix = "{$value}";
                    if (strlen($provider_name_suffix) > 40) {
                        $provider_name_suffix = substr($provider_name_suffix, 0, 37) . "...";
                    }
                    if (strcasecmp($graph_type, "boolean") == 0) {
                        $provider_name_suffix = $value ? "True" : "False";
                    }
                    $provider_name_suffix .= " - {$provider_name_suffix}";
                }
                $provider["{$provider_name} - {$provider_name_suffix}"] = $parameters;
            }
        }
        return $provider;
    }

    /**
     * Validate the data type using the result set provided
     *
     * @param Dse\Graph\ResultSet $result_set Result set to validate
     * @param string $property_name Name of the property
     * @param string $vertex_label Name of the vertex label
     * @param string $type Type of value to validate
     * @param mixed $expected Expected value found in the result set
     */
    private function validate($result_set, $property_name, $vertex_label, $type, $expected) {
        // Get the vertex from the result set
        $this->assertCount(1, $result_set);
        $result = $result_set[0];
        $this->assertTrue($result->isObject());
        $vertex = $result->asVertex();

        // Validate the properties on the vertex
        $this->assertEquals($vertex_label, $vertex->label());
        $this->assertCount(1, $vertex->properties());

        // Validate the property
        $property = $vertex->property($property_name)->value();
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

        // Get the appropriate value from the result
        if ($member->isBool()) {
            $value = $member->asBool();
        } else if ($member->isNumber()) {
            // Precision is lost with graph decimals (update expected)
            if ($type == "decimal") {
                // Determine the precision of the returned value
                $value = $member->asString();
                $precision = strlen(substr(strrchr($value, "."), 1));

                // Get the entire value from the expected decimal
                $decimal_location = strlen($expected->value()) - $expected->scale();
                $expected_value = substr_replace($expected->value(),
                    ".", $decimal_location, 0);

                // Update the expected value to utilize the updated precision
                $expected_value = round($expected_value, $precision);
                $expected = new Dse\Decimal("{$expected_value}");
            } else if ($type == "double" || $type == "float") {
                $value = $member->asDouble();
            } else {
                $value = $member->asInt();
            }
        } else if ($member->isString()) {
            if ($type == "linestring") {
                $value = $member->asLineString();
            } else if ($type == "point") {
                $value = $member->asPoint();
            } else if ($type == "polygon") {
                $value = $member->asPolygon();
            } else {
                $value = $member->asString();

                // Handle special return values from graph
                if ($type == "blob") {
                    // Blobs are returned as base64 strings in graph
                    $value = base64_decode($value);
                } else if ($type == "timestamp") {
                    // Timestamp is returned as date + time string in graph
                    $value = (new DateTime($value))->getTimestamp();
                }
            }
        }

        // Validate the value (apply coercion if necessary)
        if ($type instanceof Dse\Type) {
            $value = $type->create($value);
        }
        $this->assertEquals($expected, $value);
    }

    /**
     * Perform insert and select operations for allowable graph data types
     *
     * This test will perform multiple inserts and select operations using the
     * graph data type provider against a single node cluster using graph.
     *
     * @see https://docs.datastax.com/en/latest-dse/datastax_enterprise/graph/reference/refDSEGraphDataTypes.html
     *
     * @jira_ticket PHP-103
     * @test_category dse:graph
     * @since 1.0.0
     * @expected_result Data types are usable and retrievable
     *
     * @param string $schema Schema to create for data type
     * @param mixed $type Data type under test
     * @param mixed $value Value under test
     * @param mixed $value Expected result (validate test)
     *
     * @dataProvider data_type_provider
     * @test
     */
    public function testDataTypes($schema, $type, $value, $expected) {
        // Create the graph options and objects for the graph statement
        $unique_id = uniqid();
        $property_name = "property_" . $unique_id;
        $vertex_label = "vertex_" . $unique_id;
        $options = array(
            "graph_name" => $this->table,
            "arguments" => array(
                "property_name" => $property_name,
                "vertex_label" => $vertex_label,
                "value_field" => $value
            )
        );

        // Create the schema and insert the data type
        $this->session->executeGraph($schema, $options);
        $this->validate($this->session->executeGraph(self::INSERT, $options),
            $property_name, $vertex_label, $type, $expected);
        $this->validate($this->session->executeGraph(self::SELECT, $options),
            $property_name, $vertex_label, $type, $expected);
    }
}