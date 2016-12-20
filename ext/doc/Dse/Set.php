<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `set` datatype
 */
final class Set implements Value, Countable, Iterator {

    /**
     * Creates a new collection of a given type.
     * @param Type $type
     */
    public function __construct($type) { }

    /**
     * The type of this set.
     * @return Type
     */
    public function type() { }

    /**
     * Array of values in this set.
     * @return array values
     */
    public function values() { }

    /**
     * Adds a value to this set.
     *
     * @param mixed $value Value
     * @return bool whether the value has been added
     */
    public function add($value) { }

    /**
     * Returns whether a value is in this set.
     *
     * @param mixed $value Value
     * @return bool whether the value is in the set
     */
    public function has($value) { }

    /**
     * Removes a value to this set.
     *
     * @param mixed $value Value
     * @return bool whether the value has been removed
     */
    public function remove($value) { }

    /**
     * Total number of elements in this set
     * @return int count
     */
    public function count() { }

    /**
     * Current element for iteration
     * @return mixed current element
     */
    public function current() { }

    /**
     * Current key for iteration
     * @return int current key
     */
    public function key() { }

    /**
     * Move internal iterator forward
     * @return void
     */
    public function next() { }

    /**
     * Check whether a current value exists
     * @return bool
     */
    public function valid() { }

    /**
     * Rewind internal iterator
     * @return void
     */
    public function rewind() { }

}
