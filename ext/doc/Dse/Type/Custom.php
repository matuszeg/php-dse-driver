<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 */
final class Custom extends Type {

    private function __construct() { }

    /**
     * {@inheritDoc}
     * @return string The name of this type
     */
    public function name() { }

    /**
     * {@inheritDoc}
     * @return string String representation of this type
     */
    public function __toString() { }

    /**
     * Creation of custom type instances is not supported
     *
     * @throws Cassandra\Exception\LogicException
     * @param mixed $value the value
     * @return null nothing
     */
    public function create($value) { }

}
