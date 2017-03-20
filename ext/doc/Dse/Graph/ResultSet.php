<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A set of arbitrary data returned from a graph query. It is iterable and indexable.
 * @see Graph\Result
 */
final class ResultSet implements \Iterator, \ArrayAccess {

    private final function __construct() { }

    /**
     * Gets the number of results contained in the result set.
     *
     * @return int The number of results.
     */
    public function count() { }

    /**
     * Resets the intenal iterator of the result set to the beginning.
     *
     * @return null
     */
    public function rewind() { }

    /**
     * Gets the current value at the current position of the internal
     * iterator.
     *
     * @return mixed
     */
    public function current() { }

    /**
     * Gets the current key or offset of the current position of the
     * internal iterator;
     *
     * @return int|string
     */
    public function key() { }

    /**
     * Moves the internal iterator to the next position.
     *
     * @return null
     */
    public function next() { }

    /**
     * Determines if the internal iterator points to a valid position in the result set.
     *
     * @return bool Returns true if the internal iterator is valid.
     */
    public function valid() { }

    /**
     * Determines if a result exists at an offset.
     *
     * @param int $offset An integer offset into the result set.
     *
     * @return bool Returns true if the offset is valid.
     */
    public function offsetExists($offset) { }

    /**
     * Gets a  result at the provided offset.
     *
     * @param int $offset An integer offset into the result set.
     *
     * @return Graph\Result A graph result.
     */
    public function offsetGet($offset) { }

    /**
     * Not implmented. The result set is read-only.
     *
     * @param mixed $offset
     * @param mixed $value
     *
     * @return mixed
     */
    public function offsetSet($offset, $value) { }

    /**
     * Not implmented. The result set is read-only.
     *
     * @param mixed $offset
     *
     * @return mixed
     */
    public function offsetUnset($offset) { }

    /**
     * Gets the first result in the result set.
     *
     * @return Graph\Result The first result.
     */
    public function first() { }

}
