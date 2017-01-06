<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * Arbitrary data returned from a graph query. It can respresent several primitive types as well as composite types and graph elements.
 * @see \Dse\Graph\ResultSet
 */
final class Result implements Iterator, ArrayAccess {

    private final function __construct() { }

    /**
     * Gets the string representation of the contained data.
     * @return string A string representation of the data.
     */
    public function __toString() { }

    /**
     * Gets the number of elements in the result if it is a composite types such as an array or object.
     * @return int The number of elements.
     */
    public function count() { }

    /**
     * Resets the internal iterator of this result if it is a composite type such as an array or object.
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
    public function isBool() { }

    /**
     * @return mixed
     */
    public function isNumber() { }

    /**
     * @return mixed
     */
    public function isString() { }

    /**
     * @return mixed
     */
    public function value() { }

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
