<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL UDT datatype
 */
final class UserTypeValue implements Value, \Countable, \Iterator {

    /**
     * Creates a new user type value with the given name/type pairs.
     *
     * @param array $types Array of types
     */
    public function __construct($types) { }

    /**
     * The type of this user type value.
     *
     * @return Type
     */
    public function type() { }

    /**
     * Array of values in this user type value.
     *
     * @return array values
     */
    public function values() { }

    /**
     * Sets the value at name in this user type value.
     *
     * @param mixed $value A value or null
     *
     * @return void
     */
    public function set($value) { }

    /**
     * Retrieves the value at a given name.
     *
     * @param string $name String of the field name
     *
     * @return mixed A value or null
     */
    public function get($name) { }

    /**
     * Total number of elements in this user type value.
     *
     * @return int count
     */
    public function count() { }

    /**
     * Current element for iteration
     *
     * @return mixed The current element
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
