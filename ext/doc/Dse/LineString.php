<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Encapsulates a set of lines, characterized by a sequence of Point's in the xy-plane. It corresponds to the
 * `org.apache.cassandra.db.marshal.LineStringType` column type in DSE.
 *
 * @see https://en.wikipedia.org/wiki/Well-known_text
 */
final class LineString extends Custom  {

    /**
     * Construct a LineString with some Point's.
     *
     * To create an empty LineString:
     * @code
     *     $lineString = new Dse\LineString();
     * @endcode
     * To create with some points on the fly:
     * @code
     *     $p1 = new Dse\Point(2, 3);
     *     $p2 = new Dse\Point(4, 5);
     *     $lineString = new Dse\LineString($p1, $p2);
     * @endcode
     * To create with an array of points by unrolling into varargs:
     * @code
     *     $points = [new Dse\Point(2, 3), new Dse\Point(4, 5)];
     *     $lineString = new Dse\LineString(...$points);
     * @endcode
     *
     * @param array $points varargs-style array of Point's
     */
    public function __construct($points) { }

    /**
     * The type of this value.
     *
     * @return Type\Custom the custom type for LineString
     */
    public function type() { }

    /**
     * @return array Points that constitute this LineString
     */
    public function points() { }

    /**
     * @param int $index index of the desired Point in this LineString
     * @return Point the index'th Point in this LineString; `null` if there is no such element
     */
    public function point($index) { }

    /**
     * @return string the well-known-text representation of this LineString
     */
    public function wkt() { }

    /**
     * @return string a human-readable string representation of this LineString
     */
    public function __toString() { }

}
