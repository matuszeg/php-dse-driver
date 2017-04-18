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

        // Assign the standard composite types
        $collection = array(
            $collection_type,
            array(
                $collection_type->create(
                    $values[0],
                    $values[1],
                    $values[2]
                )
            )
        );
        $map = array(
            $map_type,
            array(
                $map_type->create(
                    0, $values[0],
                    1, $values[1],
                    2, $values[2]
                )
            )
        );
        $set = array(
            $set_type,
            array(
                $set_type->create(
                    $values[0],
                    $values[1],
                    $values[2]
                )
            )
        );

        // Determine if the expected values should be updated
        // @see https://datastax.jira.com/browse/DSP-13183 (can be branched once resolved)
        if ($data_type === Dse\Type::dateRange()) {
            // Define the rounded DateRange type values for list, map, and sets
            $round_values = array(
                $this->round_date_range($values[0]),
                $this->round_date_range($values[1]),
                $this->round_date_range($values[2])
            );

            // Add the expected values to the appropriate composite type
            $collection[] = array(
                $collection_type->create(
                    $round_values[0],
                    $round_values[1],
                    $round_values[2]
                )
            );
            $map[] = array(
                $map_type->create(
                    0, $round_values[0],
                    1, $round_values[1],
                    2, $round_values[2]
                )
            );
            $set[] = array(
                $set_type->create(
                    $round_values[0],
                    $round_values[1],
                    $round_values[2]
                )
            );
        }

        // Construct the value of the composites
        return array(
            $collection,
            $map,
            $set,
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
                if ($contains_expected) {
                    $parameters[] = $data_types[2][$index];
                } else {
                    $parameters[] = $value;
                }

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

    /**
     * Perform rounding operations on the DateRange type based on the lower and
     * upper bound and the precision of each bound
     *
     * @param Dse\DateRange $date_range Date range to perform rounding
     *                                  operations
     * @return Dse\DateRange DateRange type properly rounded based on precision
     */
    protected function round_date_range($date_range) {
        // Generate the rounded lower bound
        $lower_bound = $date_range->lowerBound();
        $lower_precision = $lower_bound->precision();
        if ($lower_precision !== Dse\DateRange\Precision::UNBOUNDED) {
            $lower_time_ms = $lower_bound->timeMs();
            $lower_round_timestamp = $this->to_lower($lower_time_ms, $lower_precision);
            $lower_bound = new Dse\DateRange\Bound($lower_precision, $lower_round_timestamp);
        }

        // Generate the rounded upper bound (if applicable to DateRange type
        if (!$date_range->isSingleDate()) {
            $upper_bound = $date_range->upperBound();
            $upper_precision = $upper_bound->precision();
            if ($upper_precision !== Dse\DateRange\Precision::UNBOUNDED) {
                $upper_time_ms = $upper_bound->timeMs();
                $upper_round_timestamp = $this->to_upper($upper_time_ms, $upper_precision);
                $upper_bound = new Dse\DateRange\Bound($upper_precision, $upper_round_timestamp);
            }

            // Return the rounded lower and upper bound DateRange type
            return new Dse\DateRange($lower_bound, $upper_bound);
        }

        // Return the rounded single DateRange type
        return new Dse\DateRange($lower_bound);
    }

    /**
     * Round down the timestamp based on the DateRange precision
     *
     * @param $time_ms Timestamp (in milliseconds)
     * @param Dse\DateRange\Precision $precision Precision to utilize when
     *                                           rounding the time
     * @return int Rounded timestamp (in milliseconds)
     */
    private function to_lower($time_ms, $precision) {
        $date_time = new DateTime();
        $date_time->setTimestamp($time_ms / 1000);

        // Round the time based on the precision
        switch ($precision) {
            case Dse\DateRange\Precision::YEAR:
                $date_time = $date_time->modify("first day of january")
                    ->setTime(0, 0, 0);
                break;
            case Dse\DateRange\Precision::MONTH:
                $date_time = $date_time->modify("first day of this month")
                    ->setTime(0, 0, 0);
                break;
            case Dse\DateRange\Precision::DAY:
                $date_time = $date_time->setTime(0, 0, 0);
                break;
            case Dse\DateRange\Precision::HOUR:
                $date_time = $date_time->setTime(
                    $date_time->format("H"),
                    0,
                    0);
                break;
            case Dse\DateRange\Precision::MINUTE:
                $date_time = $date_time->setTime(
                    $date_time->format("H"),
                    $date_time->format("i"),
                    0);
                break;
        }

        // Convert to milliseconds and return the timestamp
        return $date_time->getTimestamp() * 1000;
    }

    /**
     * Round up the timestamp based on the DateRange precision
     *
     * @param $time_ms Timestamp (in milliseconds)
     * @param Dse\DateRange\Precision $precision Precision to utilize when
     *                                           rounding the time
     * @return int Rounded timestamp (in milliseconds)
     */
    private function to_upper($time_ms, $precision) {
        $date_time = new DateTime();
        $date_time->setTimestamp($time_ms / 1000);

        // Round the time based on the precision
        switch ($precision) {
            case Dse\DateRange\Precision::YEAR:
                $date_time = $date_time->modify("last day of december")
                    ->setTime(23, 59, 60);
                break;
            case Dse\DateRange\Precision::MONTH:
                $date_time = $date_time->modify("last day of this month")
                    ->setTime(23, 59, 60);
                break;
            case Dse\DateRange\Precision::DAY:
                $date_time = $date_time->setTime(23, 59, 60);
                break;
            case Dse\DateRange\Precision::HOUR:
                $date_time = $date_time->setTime(
                    $date_time->format("H"),
                    59,
                    60);
                break;
            case Dse\DateRange\Precision::MINUTE:
                $date_time = $date_time->setTime(
                    $date_time->format("H"),
                    $date_time->format("i"),
                    60);
                break;
        }

        // Convert to milliseconds and return the timestamp
        return ($date_time->getTimestamp() * 1000) - 1;
    }
}
