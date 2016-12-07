<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
final class DefaultEdge extends Edge  {

    /**
     */
    private final function __construct() { }

    /**
     * @return mixed
     */
    public function id() { }

    /**
     * @return mixed
     */
    public function label() { }

    /**
     * @return mixed
     */
    public function properties() { }

    /**
     * @param mixed $key
     * @return mixed
     */
    public function property($key) { }

    /**
     * @return mixed
     */
    public function inV() { }

    /**
     * @return mixed
     */
    public function inVLabel() { }

    /**
     * @return mixed
     */
    public function outV() { }

    /**
     * @return mixed
     */
    public function outVLabel() { }

}
