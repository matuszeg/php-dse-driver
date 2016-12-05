<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 */
final class LineString implements Cassandra\Custom {

    /**
     * @param mixed $points 
     */
    public function __construct($points) { }

    /**
     * @return mixed 
     */
    public function type() { }

    /**
     * @return mixed 
     */
    public function points() { }

    /**
     * @param mixed $index 
     * @return mixed 
     */
    public function point($index) { }

    /**
     * @return mixed 
     */
    public function wkt() { }

    /**
     * @return mixed 
     */
    public function __toString() { }

}
