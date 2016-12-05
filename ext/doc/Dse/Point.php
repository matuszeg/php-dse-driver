<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
final class Point implements Cassandra\Custom {

    /**
     * @param mixed $x 
     * @param mixed $y 
     */
    public function __construct($x, $y) { }

    /**
     * @return mixed 
     */
    public function type() { }

    /**
     * @return mixed 
     */
    public function x() { }

    /**
     * @return mixed 
     */
    public function y() { }

    /**
     * @return mixed 
     */
    public function wkt() { }

    /**
     * @return mixed 
     */
    public function __toString() { }

}
