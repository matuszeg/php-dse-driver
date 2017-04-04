<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `tuple` datatype
 */
final class Tuple implements Value, \Countable, \Iterator {

    /**
     * Creates a new tuple with the given types.
     *
     * @param array $types Array of types
     */
    public function __construct($types) { }

    /**
     * The type of this tuple.
     *
     * @return Type
     */
    public function type() { }

    /**
     * Array of values in this tuple.
     *
     * @return array values
     */
    public function values() { }

    /**
     * Sets the value at index in this tuple .
     *
     * @param mixed $value A value or null
     *
     * @return void
     */
    public function set($value) { }

    /**
     * Retrieves the value at a given index.
     *
     * @param int $index Index
     *
     * @return mixed A value or null
     */
    public function get($index) { }

    /**
     * Total number of elements in this tuple
     *
     * @return int count
     */
    public function count() { }

    /**
     * Current element for iteration
     *
     * @return mixed current element
     */
    public function current() { }

    /**
     * Current key for iteration
     *
     * @return int current key
     */
    public function key() { }

    /**
     * Move internal iterator forward
     *
     * @return void
     */
    public function next() { }

    /**
     * Check whether a current value exists
     *
     * @return bool
     */
    public function valid() { }

    /**
     * Rewind internal iterator
     *
     * @return void
     */
    public function rewind() { }

}
