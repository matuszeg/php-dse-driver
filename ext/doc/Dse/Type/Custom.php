<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 * A class that represents a custom type.
 */
final class Custom extends Type {

    private function __construct() { }

    /**
     * Returns the name of this type as string.
     *
     * @return string The name of this type
     */
    public function name() { }

    /**
     * Returns string representation of this type.
     *
     * @return string String representation of this type
     */
    public function __toString() { }

}
