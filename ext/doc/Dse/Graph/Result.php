<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
final class Result implements Iterator, ArrayAccess {

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
     * @param mixed $offset
     * @return mixed
     */
    public function offsetExists($offset) { }

    /**
     * @param mixed $offset
     * @return mixed
     */
    public function offsetGet($offset) { }

    /**
     * @param mixed $offset
     * @param mixed $value
     * @return mixed
     */
    public function offsetSet($offset, $value) { }

    /**
     * @param mixed $offset
     * @return mixed
     */
    public function offsetUnset($offset) { }

    /**
     * @return mixed
     */
    public function isNull() { }

    /**
     * @return mixed
     */
    public function isValue() { }

    /**
     * @return mixed
     */
    public function isArray() { }

    /**
     * @return mixed
     */
    public function isObject() { }

    /**
     * @return mixed
     */
    public function asInt() { }

    /**
     * @return mixed
     */
    public function asBool() { }

    /**
     * @return mixed
     */
    public function asDouble() { }

    /**
     * @return mixed
     */
    public function asString() { }

    /**
     * @return mixed
     */
    public function asEdge() { }

    /**
     * @return mixed
     */
    public function asPath() { }

    /**
     * @return mixed
     */
    public function asVertex() { }

    /**
     * @return mixed
     */
    public function asPoint() { }

    /**
     * @return mixed
     */
    public function asLineString() { }

    /**
     * @return mixed
     */
    public function asPolygon() { }

}
