<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
abstract class VertexProperty implements Property, Element {

    /**
     * @return mixed
     */
    public abstract function name();

    /**
     * @return mixed
     */
    public abstract function value();

    /**
     * @return mixed
     */
    public abstract function parent();

    /**
     * @return mixed
     */
    public abstract function id();

    /**
     * @return mixed
     */
    public abstract function label();

    /**
     * @return mixed
     */
    public abstract function properties();

    /**
     * @param mixed $key
     * @return mixed
     */
    public abstract function property($key);

}
