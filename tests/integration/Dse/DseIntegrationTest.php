<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * Base class to provide common integration test functionality for DSE testing.
 */
abstract class DseIntegrationTest extends IntegrationTest {

    /**
     * Data provider for DSE data types
     *
     * @see DseIntegrationTest::data_types()
     *
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [
     *             [0] => (Dse\Type) Data type
     *             [1] => (string) CQL data type
     *             [3] => (mixed) Data type value
     *         ]
     *     ]
     */
    public function data_type_provider() {
        return $this->provider(false);
    }

    /**
     * Data provider for DSE data types that can be used as primary keys
     *
     * @see DseIntegrationTest::data_types()
     *
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [
     *             [0] => (Dse\Type) Data type
     *             [1] => (string) CQL data type
     *             [3] => (mixed) Data type value
     *         ]
     *     ]
     */
    public function primary_key_data_type_provider() {
        return $this->provider(true);
    }

    /**
     * Data types (composite) - DSE v5.0.0
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Dse\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires DSE >= 5.0.0
     */
    protected function composite_data_types_5_0() {
        // Iterate over the available data types and create the composite types
        $composite_types = array();
        foreach ($this->custom_data_types_5_0() as $data_types) {
            // Generate the composite type for the custom DSE data type
            $composite_types = array_merge($composite_types,
                $this->generate_composite_data_type($data_types[0], $data_types[1]));
        }
        return $composite_types;
    }

    /**
     * Data types (composite) - DSE v5.1.0
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Dse\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires DSE >= 5.1.0
     */
    protected function composite_data_types_5_1() {
        // Iterate over the available data types and create the composite types
        $composite_types = array();
        foreach ($this->custom_data_types_5_1() as $data_types) {
            // Generate the composite type for the custom DSE data type
            $composite_types = array_merge($composite_types,
                $this->generate_composite_data_type($data_types[0], $data_types[1]));
        }
        return $composite_types;
    }

    /**
     * Data types (custom) - DSE v5.0.0
     *
     * @return array Custom data type to use in a data provider
     *     [
     *         [0] => (Dse\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires DSE >= 5.0.0
     */
    protected function custom_data_types_5_0() {
        return array(
            // Point data type
            array(
                Dse\Type::point(),
                array(
                    new Dse\Point(0, 0),
                    new Dse\Point(2, 4),
                    new Dse\Point(-1.2, 100),
                )
            ),

            // Line string data type
            array(
                Dse\Type::lineString(),
                array(
                    new Dse\LineString(
                        new Dse\Point(0, 0),
                        new Dse\Point(1, 1)
                    ),
                    new Dse\LineString(
                        new Dse\Point(1, 3),
                        new Dse\Point(2, 6),
                        new Dse\Point(3, 9)
                    ),
                    new Dse\LineString(
                        new Dse\Point(-1.2, -90),
                        new Dse\Point(0.99, 3)
                    )
                )
            ),

            // Polygon data type
            array(
                Dse\Type::polygon(),
                array(
                    new Dse\Polygon(
                        new Dse\LineString(
                            new Dse\Point(1.0, 3.0),
                            new Dse\Point(3.0, 1.0),
                            new Dse\Point(3.0, 6.0),
                            new Dse\Point(1.0, 3.0)
                        )
                    ),
                    new Dse\Polygon(
                        new Dse\LineString(
                            new Dse\Point(0.0, 10.0),
                            new Dse\Point(10.0, 0.0),
                            new Dse\Point(10.0, 10.0),
                            new Dse\Point(0.0, 10.0)
                        ),
                        new Dse\LineString(
                            new Dse\Point(6.0, 7.0),
                            new Dse\Point(3.0, 9.0),
                            new Dse\Point(9.0, 9.0),
                            new Dse\Point(6.0, 7.0)
                        )
                    ),
                    new Dse\Polygon(
                        new Dse\LineString(
                            new Dse\Point(0.0, 0.0),
                            new Dse\Point(5.0, 0.0),
                            new Dse\Point(5.0, 3.0),
                            new Dse\Point(0.0, 3.0),
                            new Dse\Point(0.0, 0.0)
                        ),
                        new Dse\LineString(
                            new Dse\Point(1.0, 1.0),
                            new Dse\Point(1.0, 2.0),
                            new Dse\Point(2.0, 2.0),
                            new Dse\Point(2.0, 1.0),
                            new Dse\Point(1.0, 1.0)
                        ),
                        new Dse\LineString(
                            new Dse\Point(3.0, 1.0),
                            new Dse\Point(3.0, 2.0),
                            new Dse\Point(4.0, 2.0),
                            new Dse\Point(4.0, 1.0),
                            new Dse\Point(3.0, 1.0)
                        )
                    )
                )
            )
        );
    }

    /**
     * Data types (custom) - DSE v5.1.0
     *
     * @return array Custom data type to use in a data provider
     *     [
     *         [0] => (Dse\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires DSE >= 5.1.0
     */
    protected function custom_data_types_5_1() {
        // Create dates to be used by DateRange data types
        $date_one = new DateTime("2017-01-02T03:04:05.876");
        $date_two = new DateTime("2017-11-02T03:04:05.876");

        return array(
            // DateRange data type
            array(
                Dse\Type::dateRange(),
                array(
                    new Dse\DateRange(
                        Dse\DateRange\Bound::unbounded()
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Precision::YEAR, $date_one
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Precision::MONTH, $date_one,
                        Dse\DateRange\Precision::DAY, $date_two
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Bound::unbounded(),
                        Dse\DateRange\Precision::HOUR, $date_two
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Precision::MINUTE, $date_one,
                        Dse\DateRange\Bound::unbounded()
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Bound::unbounded(),
                        Dse\DateRange\Bound::unbounded()
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Precision::SECOND, $date_one,
                        Dse\DateRange\Precision::MILLISECOND, $date_two
                    ),
                    new Dse\DateRange(
                        Dse\DateRange\Precision::SECOND, -9234123,
                        Dse\DateRange\Precision::MILLISECOND, "1491935325000"
                    )
                )
            )
        );
    }

    /**
     * DSE data types
     *
     * @see IntegrationTest::data_types()
     * @see DseIntegrationTest::custom_data_types_5_0() (if DSE >= 5.0.0)
     * @see DseIntegrationTest::composite_data_types_5_0() (if DSE >= 5.0.0)
     * @see DseIntegrationTest::custom_data_types_5_1() (if DSE >= 5.1.0)
     *
     * @param bool $primary_keys (Optional) True if data types will be used as
     *                                      a primary key; false otherwise
     *                                      (default: false)
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [0] => (Dse\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function data_types($primary_keys = false) {
        // Get the data types from the parent class (Core data types)
        $data_types = parent::data_types($primary_keys);

        // Return all the valid composite and custom data types for DSE version
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version) {
            if ($version->compare("5.0.0") >= 0) {
                $data_types = array_merge($data_types,
                    $this->custom_data_types_5_0(),
                    $this->composite_data_types_5_0());
            }
            if ($version->compare("5.1.0") >= 0 && !$primary_keys) {
                $data_types = array_merge($data_types,
                    $this->custom_data_types_5_1(),
                    $this->composite_data_types_5_1());
            }
        }
        return $data_types;
    }

    /**
     * Generate the CQL data type from the give data type
     *
     * @param Dse\Type $type Data type
     * @return string String representation of the CQL data type
     */
    protected function generate_cql_data_type($type) {
        // Determine if the data type is a geospatial data type (shorten name)
        if ($type === Dse\Type::lineString()) {
            return "'LineStringType'";
        } else if ($type === Dse\Type::point()) {
            return "'PointType'";
        } else if ($type === Dse\Type::polygon()) {
            return "'PolygonType'";
        } else if ($type === Dse\Type::dateRange()) {
            return "'DateRangeType'";
        }

        // Ensure composites are frozen for backwards compatibility
        if ($this->is_composite_data_type($type)) {
            // Gather the value types from the composite type
            $types = array();
            if ($type instanceof Dse\Type\Collection ||
                $type instanceof Dse\Type\Set
            ) {
                $types[] = $type->valueType();
            } else if ($type instanceof Dse\Type\Map) {
                $types[] = $type->keyType();
                $types[] = $type->valueType();
            } else if ($type instanceof Dse\Type\Tuple) {
                $types = $type->types();
            }

            // Determine if the composite contains any DSE custom types
            $custom_types = array(
                Dse\Type::lineString(),
                Dse\Type::point(),
                Dse\Type::polygon(),
                Dse\Type::dateRange()
            );
            foreach($custom_types as $custom_type) {
                if (in_array($custom_type, $types)) {
                    return sprintf("frozen<%s>", (string) $type);
                }
            }
        }

        // Call the parent class to determine CQL data type
        return parent::generate_cql_data_type($type);
    }

    protected function generate_composite_data_type($data_type, $values) {
        // Ensure there are at least three values available
        if (count($values) < 3) {
            throw new Exception("Invalid Number of Values: "
                . "{$data_type} must have at least 3 values");
        }

        // Define the composite data types
        $collection_type = Dse\Type::collection($data_type);
        $map_type = Dse\Type::map(
            Dse\Type::int(),
            $data_type);
        $set_type = Dse\Type::set($data_type);
        $tuple_type = Dse\Type::tuple(
            $data_type,
            $data_type,
            $data_type
        );
        $user_data_type_type = Dse\Type::userType(
            "a", $data_type,
            "b", $data_type,
            "c", $data_type
        );
        $name = strtolower(
            $this->generate_user_data_type_name($user_data_type_type)
        );
        $user_data_type_type = $user_data_type_type->withName($name);

        // Construct the value of the composites
        return array(
            array(
                $collection_type,
                array(
                    $collection_type->create(
                        $values[0],
                        $values[1],
                        $values[2]
                    )
                )
            ),
            array(
                $map_type,
                array(
                    $map_type->create(
                        0, $values[0],
                        1, $values[1],
                        2, $values[2]
                    )
                )
            ),
            array(
                $set_type,
                array(
                    $set_type->create(
                        $values[0],
                        $values[1],
                        $values[2]
                    )
                )
            ),
            array(
                $tuple_type,
                array(
                    $tuple_type->create(
                        $values[0],
                        $values[1],
                        $values[2]
                    )
                )
            ),
            array(
                $user_data_type_type,
                array(
                    $user_data_type_type->create(
                        "a", $values[0],
                        "b", $values[1],
                        "c", $values[2]
                    )
                )
            )
        );
    }

    /**
     * Data provider for DSE data types
     *
     * @see DseIntegrationTest::data_types()
     *
     * @param bool $primary_keys True if data types will be used as a primary
     *                           key; false otherwise
     * @return array Composite and scalar data type to use in a data provider
     *     [
     *         [
     *             [0] => (Dse\Type) Data type
     *             [1] => (string) CQL data type
     *             [3] => (mixed) Data type value
     *         ]
     *     ]
     */
    protected function provider($primary_keys) {
        // Expand data types into a provider
        $provider = array();
        $count = 1;
        foreach ($this->data_types($primary_keys) as $data_types) {
            // Get the data type and CQL data type
            $type = $data_types[0];
            $cql_type = $this->generate_cql_data_type($type);

            // Iterate over each value and update the provider (flatten array)
            foreach ($data_types[1] as $index => $value) {
                // Create the parameters for the provider
                $parameters = array();
                $parameters[] = $type;
                $parameters[] = $cql_type;
                $parameters[] = $value;

                // Name the provider and add the parameters
                $provider_name = "{$cql_type}";
                $provider_name_suffix = "#" . $count++;
                if (IntegrationTestFixture::get_instance()->configuration->verbose) {
                    if (!$this->is_composite_data_type($type)) {
                        $provider_name_suffix = "{$value}";
                        if (strlen($provider_name_suffix) > 40) {
                            $provider_name_suffix = substr($provider_name_suffix, 0, 37) . "...";
                        }
                        if (strcasecmp($cql_type, "boolean") == 0) {
                            $provider_name_suffix = $value ? "True" : "False";
                        }
                        $provider_name_suffix .= " - {$provider_name_suffix}";
                    }
                }
                $provider["{$provider_name} - {$provider_name_suffix}"] = $parameters;
            }
        }
        return $provider;
    }
}