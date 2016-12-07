<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Encapsulates a set of lines, characterized by a sequence of {Point}s in the xy-plane. It corresponds to the
 * `org.apache.cassandra.db.marshal.LineStringType` column type in DSE.
 * 
 * @see https://en.wikipedia.org/wiki/Well-known_text Wikipedia article on Well Known Text
 */
final class LineString implements Cassandra\Custom {

    /**
     * Construct a LineString with some {Point}s.
     * @example Create with some points on the fly
     *     $p1 = new Dse\Point(2, 3);
     *     $p2 = new Dse\Point(4, 5);
     *     $lineString = new Dse\LineString($p1, $p2);
     * @example Create with an array of points by unrolling into varargs
     *     $points = [new Dse\Point(2, 3), new Dse\Point(4, 5)];
     *     $lineString = new Dse\LineString(...$points);
     * @param Array<Point> $points Varargs-style array of {Point}s
     */
    public function __construct($points) { }

    /**
     * @return Cassandra\Type\Custom the Cassandra custom type for LineString
     */
    public function type() { }

    /**
     * @return Array<Point> {Point}s that constitute this LineString
     */
    public function points() { }

    /**
     * @param int $index index of the desired Point in this LineString
     * @return Point the index'th Point in this LineString; `null` if there is no such element
     */
    public function point($index) { }

    /**
     * @return String The well-known-text representation of this LineString
     */
    public function wkt() { }

    /**
     * @return String A human-readable string representation of this LineString
     */
    public function __toString() { }

}
