<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 */
final class Collection extends Type {

    private function __construct() { }

    /**
     * Returns "list"
     * @return string "list"
     */
    public function name() { }

    /**
     * Returns type of values
     * @return Type Type of values
     */
    public function valueType() { }

    /**
     * Returns type representation in CQL, e.g. `list<varchar>`
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Creates a new Cassandra\Collection from the given values.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different type than what this
     *                                            list type expects.
     *
     *                               When no values given, creates an empty list.
     * @param mixed $value ,...      One or more values to be added to the list.
     * @return Cassandra\Collection A list with given values.
     */
    public function create($value) { }

}
