<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A connection between two vertices. It contains a reference to the two
 * vertices as well as a colleciton of properties.
 *
 * @see Graph\Vertex
 */
final class DefaultEdge extends Edge  {

    private final function __construct() { }

    /**
     * Gets the edge's unique identifier.
     *
     * @return Graph\Result The edge's identifier as a graph result.
     */
    public function id() { }

    /**
     * Gets the edge's label.
     *
     * @return string The edge's label.
     */
    public function label() { }

    /**
     * Gets the edge's properties as an array of graph results.
     *
     * @return array An array of graph results.
     */
    public function properties() { }

    /**
     * Gets an edge's property for a specific key.
     *
     * @param string $key A property's name
     *
     * @return Graph\Result An edge's property as a graph result.
     */
    public function property($key) { }

    /**
     * Gets the "in" vertex.
     *
     * @return Graph\Result A graph vertex as a graph result.
     */
    public function inV() { }

    /**
     * Gets the label of the "in" vertex.
     *
     * @return string The label of the "in" vertex.
     */
    public function inVLabel() { }

    /**
     * Gets the "out" vertex
     *
     * @return Graph\Result A graph vertex as a graph result.
     */
    public function outV() { }

    /**
     * Gets the label of the "out" vertex.
     *
     * @return string The label of the "out" vertex
     */
    public function outVLabel() { }

}
