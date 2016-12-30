<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `map` datatype
 */
final class Map implements Value, Countable, Iterator, ArrayAccess {

    /**
     * Creates a new map of a given key and value type.
     * @param Type $keyType
     * @param Type $valueType
     */
    public function __construct($keyType, $valueType) { }

    /**
     * The type of this map.
     * @return Type
     */
    public function type() { }

    /**
     * Returns all keys in the map as an array.
     * @return array keys
     */
    public function keys() { }

    /**
     * Returns all values in the map as an array.
     * @return array values
     */
    public function values() { }

    /**
     * Sets key/value in the map.
     * @param mixed $key key
     * @param mixed $value value
     * @return mixed
     */
    public function set($key, $value) { }

    /**
     * Gets the value of the key in the map.
     *
     * @param mixed $key Key
     * @return mixed Value or null
     */
    public function get($key) { }

    /**
     * Removes the key from the map.
     *
     * @param mixed $key Key
     * @return bool Whether the key was removed or not, e.g. didn't exist
     */
    public function remove($key) { }

    /**
     * Returns whether the key is in the map.
     *
     * @param mixed $key Key
     * @return bool Whether the key is in the map or not
     */
    public function has($key) { }

    /**
     * Total number of elements in this map
     * @return int count
     */
    public function count() { }

    /**
     * Current value for iteration
     * @return mixed current value
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

    /**
     * Sets the value at a given key
     *
     * @throws Exception\InvalidArgumentException when the type of key or value is wrong
     *
     * @param mixed $key Key to use.
     * @param mixed $value Value to set.
     * @return void
     */
    public function offsetSet($key, $value) { }

    /**
     * Retrieves the value at a given key
     *
     * @throws Exception\InvalidArgumentException when the type of key is wrong
     * @param mixed $key Key to use.
     * @return mixed Value or `null`
     */
    public function offsetGet($key) { }

    /**
     * Deletes the value at a given key
     *
     * @throws Exception\InvalidArgumentException when the type of key is wrong
     *
     * @param mixed $key Key to use.
     * @return void
     */
    public function offsetUnset($key) { }

    /**
     * Returns whether the value a given key is present
     *
     * @throws Exception\InvalidArgumentException when the type of key is wrong
     *
     * @param mixed $key Key to use.
     * @return bool Whether the value at a given key is present
     */
    public function offsetExists($key) { }

}