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
    public function first() { }

}
