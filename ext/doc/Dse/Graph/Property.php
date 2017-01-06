<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A property or sub-property of a graph element.
 */
interface Property {

    /**
     * Gets the name of the property.
     * @return string The string name of a property.
     */
    public function name();

    /**
     * Gets the value of the property.
     * @return Graph\Result The value of the property.
     */
    public function value();

    /**
     * Gets The graph element that contains this property.
     * @return Graph\Result The parent graph element of this property.
     */
    public function parent();

}
