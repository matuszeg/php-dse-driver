<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Encapsulates a 2D point with x,y coordinates. It corresponds to the
 * `org.apache.cassandra.db.marshal.PointType` column type in DSE.
 *
 * @see https://en.wikipedia.org/wiki/Well-known_text
 */
final class Point extends Custom  {

    /**
     * @param double|string $pointData Either x, y pair or a WKT string
     */
    public function __construct($pointData) { }

    /**
     * @return Dse\Type\Custom the custom type for Point
     */
    public function type() { }

    /**
     * @return double the x-coordinate of this Point
     */
    public function x() { }

    /**
     * @return double the y-coordinate of this Point
     */
    public function y() { }

    /**
     * @return string the well-known-text representation of this Point
     */
    public function wkt() { }

    /**
     * @return string a human-readable string representation of this Point
     */
    public function __toString() { }

}
