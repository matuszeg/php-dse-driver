<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
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
     *             [0] => (Dse\Type|string) Data type
     *             [1] => (string) CQL data type
     *             [2] => (Dse\Type\Timeuuid) Timeuuid generated value
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
     *             [0] => (Dse\Type|string) Data type
     *             [1] => (string) CQL data type
     *             [2] => (Dse\Type\Timeuuid) Timeuuid generated value
     *             [3] => (mixed) Data type value
     *         ]
     *     ]
     */
    public function primary_key_data_type_provider() {
        return $this->provider(true);
    }

    /**
     * Data provider for DSE data types
     *
     * @see IntegrationTest::data_types()
     * @see DseIntegrationTest::geometry_data_types() (if DSE >= 5.0.0)
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
        // Determine if the geometry data types should be added
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version && $version->compare("5.0.0") >= 0) {
            return array_merge(parent::data_types($primary_keys), $this->geometry_data_types());
        }
        return parent::data_types($primary_keys);
    }

    /**
     * Generate the CQL data type from the give data type
     *
     * @param Dse\Type|string $type Data type
     * @return string String representation of the CQL data type
     */
    protected function generate_cql_data_type($type) {
        // Determine if the data type is a geospatial data type
        if (strcasecmp("linestring", $type) == 0) {
            return "'LineStringType'";
        } else if (strcasecmp("point", $type) == 0) {
            return "'PointType'";
        } else if (strcasecmp("polygon", $type) == 0) {
            return "'PolygonType'";
        }

        // Call the parent class to determine CQL data type
        return parent::generate_cql_data_type($type);
    }

    /**
     * DSE data types (Geometry)
     *
     * @return array Scalar data type to use in a data provider
     *     [
     *         [0] => (string) Geometry data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function geometry_data_types() {
        return array(
            // Point data type
            array(
                "point",
                array(
                    new Dse\Point(0, 0),
                    new Dse\Point(2, 4),
                    new Dse\Point(-1.2, 100),
                )
            ),

            // Line string data type
            array(
                "linestring",
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
                        new Dse\Point(-1.2, -100),
                        new Dse\Point(0.99, 3)
                    )
                )
            ),

            // Polygon data type
            array(
                "polygon",
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
     *             [0] => (Dse\Type|string) Data type
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