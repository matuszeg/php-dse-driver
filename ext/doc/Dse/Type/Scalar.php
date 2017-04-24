<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 * A class that represents a primitive type (e.g. `varchar` or `bigint`)
 */
final class Scalar extends Type {

    private function __construct() { }

    /**
     * Returns the name of this type as string.
     *
     * @return string Name of this type
     */
    public function name() { }

    /**
     * Returns string representation of this type.
     *
     * @return string String representation of this type
     */
    public function __toString() { }

    /**
     * @param mixed $value
     * @return mixed
     */
    public function create($value) { }

}
