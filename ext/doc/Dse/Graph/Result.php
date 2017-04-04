<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * Arbitrary data returned from a graph query. It can respresent several primitive types as well as composite types and graph elements.
 * @see Graph\ResultSet
 */
final class Result implements \Iterator, \ArrayAccess {

    private final function __construct() { }

    /**
     * Gets the string representation of the contained data.
     *
     * @return string A string representation of the data.
     */
    public function __toString() { }

    /**
     * Gets the number of elements in the result if it is a composite types such as an array or object.
     *
     * @return int The number of elements.
     */
    public function count() { }

    /**
     * Resets the internal iterator of this result if it is a composite type
     * such as an array or object.
     *
     * @return null
     */
    public function rewind() { }

    /**
     * Gets the element of the composite result at the current position of the
     * iterator.
     *
     * @return Graph\Result The current element.
     */
    public function current() { }

    /**
     * Gets the key of an object result at the current posistion of the
     * iterator.
     *
     * @return string The current key.
     */
    public function key() { }

    /**
     * Moves the iterator to the next position if a composite result such as an array or object.
     *
     * @return null
     */
    public function next() { }

    /**
     * Determines if the iterator is currently in a valid position.
     *
     * @return bool `true` if the current iterator position is valid.
     */
    public function valid() { }

    /**
     * Determines if an offset is contained in the result. Only valid
     * for an array or object.
     *
     * @param int|string $offset An integer offset for array or a string key for objects.
     *
     * @return bool Returns true if the offset or key is valid.
     */
    public function offsetExists($offset) { }

    /**
     * Gets the value contained at an offset. Only valid for an array or object.
     *
     * @param Graph\Result $offset The value at the offset.
     *
     * @return mixed
     */
    public function offsetGet($offset) { }

    /**
     * Not implmented. The result is read-only.
     *
     * @param mixed $offset
     * @param mixed $value
     *
     * @return mixed
     */
    public function offsetSet($offset, $value) { }

    /**
     * Not implmented. The result is read-only.
     *
     * @param mixed $offset
     *
     * @return mixed
     */
    public function offsetUnset($offset) { }

    /**
     * Determines if the result is a null value.
     *
     * @return bool Returns true if the result is a null.
     */
    public function isNull() { }

    /**
     * Determines if the result is not a composite value like an array or object.
     *
     * @return bool Returns true if the result is not an array or object.
     */
    public function isValue() { }

    /**
     * Determines if the result is an array.
     *
     * @return bool Returns true if the result is an array.
     */
    public function isArray() { }

    /**
     * Determines if the result is an object.
     *
     * @return bool Returns true if the result is an object.
     */
    public function isObject() { }

    /**
     * Determines if the result is a bool.
     *
     * @return bool Returns true if the result is a bool.
     */
    public function isBool() { }

    /**
     * Determines if the value is an integer or a double.
     *
     * @return bool Returns true if the result is an integer or a double.
     */
    public function isNumber() { }

    /**
     * Determines if the value is a string.
     *
     * @return bool Returns true if the result is a string.
     */
    public function isString() { }

    /**
     * Gets the PHP value contained in the result.
     *
     * @return mixed The value.
     */
    public function value() { }

    /**
     * Determines if the result is a integer.
     *
     * @return bool Returns true if the result is an integer.
     */
    public function asInt() { }

    /**
     * Converts the result's value to a bool.
     *
     * @return bool A bool representation of the result's value.
     */
    public function asBool() { }

    /**
     * Converts the result's value to a double.
     *
     * @return double A double representation of the result's value.
     */
    public function asDouble() { }

    /**
     * Converts the result's value to a string.
     *
     * @return string A string representation of the result's value.
     */
    public function asString() { }

    /**
     * Converts an object result to an edge.
     *
     * @throws Exception\DomainException
     *
     * @return Graph\Edge A graph edge.
     */
    public function asEdge() { }

    /**
     * Converts an object result to a path.
     *
     * @throws Exception\DomainException
     *
     * @return Graph\Path A graph path.
     */
    public function asPath() { }

    /**
     * Converts an object result to a vertex.
     *
     * @throws Exception\DomainException
     *
     * @return Grap\Vertex A graph vertex.
     */
    public function asVertex() { }

    /**
     * Converts a string result to the point geospatial type. This
     * expects the string to be in the well-known text (WKT) representation.
     *
     * @throws Exception\DomainException
     *
     * @return Point A geospatial point value.
     */
    public function asPoint() { }

    /**
     * Converts a string result to the line string geospatial type. This
     * expects the string to be in the well-known text (WKT) representation.
     *
     * @throws Exception\DomainException
     *
     * @return LineString A geospatial line string value.
     */
    public function asLineString() { }

    /**
     * Converts a string result to the polygon geospatial type. This
     * expects the string to be in the well-known text (WKT) representation.
     *
     * @throws Exception\DomainException
     *
     * @return Polygon A geospatial polygon value.
     */
    public function asPolygon() { }

}
