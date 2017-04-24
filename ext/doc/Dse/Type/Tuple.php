<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 * A class that represents the tuple type. The tuple type is able to represent
 * a composite type of one or more types accessed by index.
 */
final class Tuple extends Type {

    private function __construct() { }

    /**
     * Returns "tuple"
     *
     * @return string "tuple"
     */
    public function name() { }

    /**
     * Returns type representation in CQL, e.g. `tuple<varchar, int>`
     *
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Returns types of values
     *
     * @return array An array of types
     */
    public function types() { }

    /**
     * Creates a new Tuple from the given values. When no values given,
     * creates a tuple with null for the values.
     *
     * @param mixed $values ,... One or more values to be added to the tuple.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different type than what the
     *                                            tuple expects.
     *
     * @return Tuple A tuple with given values.
     */
    public function create($values) { }

}
