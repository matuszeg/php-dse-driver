<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A sequence of graph elements that represents a path between two vertices.
 *
 * @see Graph\Edge
 * @see Graph\Vertex
 */
final class DefaultPath extends Path {

    private final function __construct() { }

    /**
     * A sequence of string labels representing the path of graph elements
     * between two vertices.
     *
     * @return array An array of arrays of string labels.
     */
    public function labels() { }

    /**
     * Gets an array of graph elements between the two vertices.
     *
     * @return array An array of graph results between the two vertices.
     */
    public function objects() { }

    /**
     * Determines if the following path contains a label.
     *
     * @return mixed
     */
    public function hasLabel() { }

    /**
     * Get an object in the path with a particular label.
     *
     * @param string $key A string label of an object.
     *
     * @return Graph\Result|false The graph element as a graph result or `false` if the label is not in
     *                            the path.
     */
    public function object($key) { }

}
