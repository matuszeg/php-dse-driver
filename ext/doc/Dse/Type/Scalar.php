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
     * {@inheritDoc}
     *
     * @return string {@inheritDoc}
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string {@inheritDoc}
     */
    public function __toString() { }

    /**
     * @param mixed $value
     * @return mixed
     */
    public function create($value) { }

}
