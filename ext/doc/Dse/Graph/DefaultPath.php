<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
final class DefaultPath {

    private final function __construct() { }

    /**
     * @return mixed
     */
    public function labels() { }

    /**
     * @return mixed
     */
    public function objects() { }

    /**
     * @return mixed
     */
    public function hasLabel() { }

    /**
     * @param mixed $key
     * @return mixed
     */
    public function object($key) { }

}
