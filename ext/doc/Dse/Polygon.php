<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
final class Polygon implements Cassandra\Custom {

    /**
     * @param mixed $rings 
     */
    public function __construct($rings) { }

    /**
     * @return mixed 
     */
    public function type() { }

    /**
     * @return mixed 
     */
    public function exteriorRing() { }

    /**
     * @return mixed 
     */
    public function interiorRings() { }

    /**
     * @param mixed $index 
     * @return mixed 
     */
    public function interiorRing($index) { }

    /**
     * @return mixed 
     */
    public function wkt() { }

    /**
     * @return mixed 
     */
    public function __toString() { }

}
