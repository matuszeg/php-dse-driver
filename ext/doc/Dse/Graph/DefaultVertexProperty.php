<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A property of a graph vertex. A vertex property can contain its own
 * properties.
 */
final class DefaultVertexProperty extends VertexProperty  {

    private final function __construct() { }

    /**
     * Gets the name of the property.
     *
     * @return string The string name of a property.
     */
    public function name() { }

    /**
     * Gets the value of the property.
     *
     * @return Graph\Result The value of the property.
     */
    public function value() { }

    /**
     * Gets The graph element that contains this property.
     *
     * @return Graph\Result The parent graph element of this property.
     */
    public function parent() { }

    /**
     * Gets the vertex property's unique identifier.
     *
     * @return Graph\Result The vertex property's identifier as a graph result.
     */
    public function id() { }

    /**
     * Get's the vertex property's label.
     *
     * @return string The vertex property's label.
     */
    public function label() { }

    /**
     * Gets an array of the vertex property's properties.
     *
     * @return array An array of graph results.
     */
    public function properties() { }

    /**
     * Gets the vertex property's property at the specified key.
     *
     * @param string $key The name of the vertex property's property.
     *
     * @return Graph\Result The vertex property's property as a graph result.
     */
    public function property($key) { }

}
