<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A graph element that is the basis of the graph and can be connected to other
 * vertices using graph edges.
 */
final class DefaultVertex extends Vertex  {

    private final function __construct() { }

    /**
     * Gets the vertex's unique identifier.
     *
     * @return Graph\Result The vertex's identifier as a graph result.
     */
    public function id() { }

    /**
     * Gets the vertex's label.
     *
     * @return string The vertex's label.
     */
    public function label() { }

    /**
     * Gets the vertex's properties as an array of graph results.
     *
     * @return array An array of graph results.
     */
    public function properties() { }

    /**
     * Gets an vertex's property for a specific key.
     *
     * @param string $key A property's name
     *
     * @return Graph\Result An vertex's property as a graph result.
     */
    public function property($key) { }

}
