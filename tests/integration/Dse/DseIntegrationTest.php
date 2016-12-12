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
     * DSE data types (Geometry)
     *
     * NOTE: Should be used as a data provider
     */
    public function geometry_data_types() {
        return array(
            // Point data type
            array(
                "point",
                array(
                    new \Dse\Point(0, 0),
                    new \Dse\Point(2, 4),
                    new \Dse\Point(-1.2, 100),
                )
            ),

            // Line string data type
            array(
                "linestring",
                array(
                    new \Dse\LineString(
                        new \Dse\Point(0, 0),
                        new \Dse\Point(1, 1)
                    ),
                    new \Dse\LineString(
                        new \Dse\Point(1, 3),
                        new \Dse\Point(2, 6),
                        new \Dse\Point(3, 9)
                    ),
                    new \Dse\LineString(
                        new \Dse\Point(-1.2, -100),
                        new \Dse\Point(0.99, 3)
                    )
                )
            ),

            // Polygon data type
            array(
                "polygon",
                array(
                    new \Dse\Polygon(
                        new \Dse\LineString(
                            new \Dse\Point(1.0, 3.0),
                            new \Dse\Point(3.0, 1.0),
                            new \Dse\Point(3.0, 6.0),
                            new \Dse\Point(1.0, 3.0)
                        )
                    ),
                    new \Dse\Polygon(
                        new \Dse\LineString(
                            new \Dse\Point(0.0, 10.0),
                            new \Dse\Point(10.0, 0.0),
                            new \Dse\Point(10.0, 10.0),
                            new \Dse\Point(0.0, 10.0)
                        ),
                        new \Dse\LineString(
                            new \Dse\Point(6.0, 7.0),
                            new \Dse\Point(3.0, 9.0),
                            new \Dse\Point(9.0, 9.0),
                            new \Dse\Point(6.0, 7.0)
                        )
                    )
                )
            )
        );
    }
}