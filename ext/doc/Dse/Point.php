<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Encapsulates a 2D point with x,y coordinates. It corresponds to the `org.apache.cassandra.db.marshal.PointType`
 * column type in DSE.
 * @see https://en.wikipedia.org/wiki/Well-known_text Wikipedia article on Well Known Text"
 */
final class Point implements Cassandra\Custom {

    /**
     * @param double $x The x-coordinate of the point
     * @param double $y The y-coordinate of the point
     */
    public function __construct($x, $y) { }

    /**
     * @return Cassandra\Type\Custom the Cassandra custom type for Point
     */
    public function type() { }

    /**
     * @return double The x-coordinate of this Point
     */
    public function x() { }

    /**
     * @return double The y-coordinate of this Point
     */
    public function y() { }

    /**
     * @return String The well-known-text representation of this Point
     */
    public function wkt() { }

    /**
     * @return String A human-readable string representation of this Point
     */
    public function __toString() { }

}
