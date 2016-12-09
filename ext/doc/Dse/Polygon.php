<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Encapsulates a polygon consisting of a set of linear-rings in the xy-plane. It corresponds to the
 * `org.apache.cassandra.db.marshal.PolygonType` column type in DSE.
 *
 * A linear-ring is a LineString whose last point is the same as its first point. The first ring specified
 * in a polygon defines the outer edges of the polygon and is called the _exterior ring_. A polygon may also have
 * _holes_ within it, specified by other linear-rings, and those holes may contain linear-rings indicating
 * _islands_. All such rings are called _interior rings_.
 *
 * @see https://en.wikipedia.org/wiki/Well-known_text
 */
final class Polygon implements Cassandra\Custom {

    /**
     * To construct an empty Polygon:
     * @code
     *   $polygon = new Dse\Polygon();
     * @endcode
     * To construct a Polygon with LineString objects:
     * @code
     *   $exterior_ring = new Dse\LineString(new Dse\Point(0, 0), new Dse\Point(10, 0), new Dse\Point(10, 10), new Dse\Point(0, 0));
     *   $interior_ring = new Dse\LineString(new Dse\Point(1, 1), new Dse\Point(1, 5), new Dse\Point(5, 1), new Dse\Point(1, 1));
     *   $polygon = new Dse\Polygon.new($exterior_ring, $interior_ring);
     * @endcode
     *
     * param array $rings varargs-style array of LineStrings
     */
    public function __construct($rings) { }

    /**
     * @return Cassandra\Type\Custom the Cassandra custom type for Polygon
     */
    public function type() { }

    /**
     * @return LineString the linear-ring characterizing the exterior of this Polygon. `null` for empty polygon.
     */
    public function exteriorRing() { }

    /**
     * @return array LineStrings that constitute this Polygon
     */
    public function rings() { }

    /**
     * @return array LineStrings that constitute the interior rings of this Polygon
     */
    public function interiorRings() { }

    /**
     * param int $index index of the desired LineString in this Polygon
     * @return LineString the index'th LineString in this Polygon; `null` if there is no such element
     */
    public function ring($index) { }

    /**
     * @return string the well-known-text representation of this Polygon
     */
    public function wkt() { }

    /**
     * @return string a human-readable string representation of this Polygon
     */
    public function __toString() { }

}
