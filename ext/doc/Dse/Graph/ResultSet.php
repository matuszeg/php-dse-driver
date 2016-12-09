<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
final class ResultSet implements Iterator, ArrayAccess {

    /**
     */
    private final function __construct() { }

    /**
     * @return mixed
     */
    public function count() { }

    /**
     * @return mixed
     */
    public function rewind() { }

    /**
     * @return mixed
     */
    public function current() { }

    /**
     * @return mixed
     */
    public function key() { }

    /**
     * @return mixed
     */
    public function next() { }

    /**
     * @return mixed
     */
    public function valid() { }

    /**
     * @return mixed
     */
    public function offsetExists($offset) { }

    /**
     * @return mixed
     */
    public function offsetGet($offset) { }

    /**
     * @return mixed
     */
    public function offsetSet($offset, $value) { }

    /**
     * @return mixed
     */
    public function offsetUnset($offset) { }

    /**
     * @return mixed
     */
    public function first() { }

}
