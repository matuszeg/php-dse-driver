<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A representation of a graph element
 */
final class DefaultElement implements Element {

    private final function __construct() { }

    /**
     * Gets the element's unique identifier.
     *
     * @return Graph\Result The element's identifier as a graph result.
     */
    public function id() { }

    /**
     * Get's the element's label.
     *
     * @return string The element's label.
     */
    public function label() { }

    /**
     * Gets an array of the element's properties.
     *
     * @return array An array of graph results.
     */
    public function properties() { }

    /**
     * Gets the element's property at the specified key.
     *
     * @param string $key The name of the element's property.
     *
     * @return Graph\Result The element's property as a graph result.
     */
    public function property($key) { }

}
