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
 * A linear-ring is a {LineString} whose last point is the same as its first point. The first ring specified
 * in a polygon defines the outer edges of the polygon and is called the _exterior ring_. A polygon may also have
 * _holes_ within it, specified by other linear-rings, and those holes may contain linear-rings indicating
 * _islands_. All such rings are called _interior rings_.
 * 
 * @see https://en.wikipedia.org/wiki/Well-known_text Wikipedia article on Well Known Text
 */
final class Polygon implements Cassandra\Custom {

    /**
     * @example Construct an empty Polygon
     *   $polygon = new Polygon();
     * @example Construct a Polygon with LineString objects
     *   $exterior_ring = new LineString(new Point(0, 0), new Point(10, 0), new Point(10, 10), new Point(0, 0));
     *   $interior_ring = new LineString(new Point(1, 1), new Point(1, 5), new Point(5, 1), new Point(1, 1));
     *   $polygon = new Polygon.new($exterior_ring, $interior_ring);
     * @param Array<LineString> $rings Varargs-style array of {LineString}s
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
     * @return Array<LineString> {LineString}s that constitute this Polygon
     */
    public function rings() { }

    /**
     * @return Array<LineString> {LineString}s that constitute the interior rings of this Polygon
     */
    public function interiorRings() { }

    /**
     * @param int $index index of the desired LineString in this Polygon
     * @return LineString the index'th LineString in this Polygon; `null` if there is no such element
     */
    public function ring($index) { }

    /**
     * @return String The well-known-text representation of this Polygon
     */
    public function wkt() { }

    /**
     * @return String A human-readable string representation of this Polygon
     */
    public function __toString() { }

}
