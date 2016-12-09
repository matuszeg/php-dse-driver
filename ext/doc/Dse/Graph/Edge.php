<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
abstract class Edge implements Element {

    /**
     * @return mixed
     */
    public abstract function inV();

    /**
     * @return mixed
     */
    public abstract function inVLabel();

    /**
     * @return mixed
     */
    public abstract function outV();

    /**
     * @return mixed
     */
    public abstract function outVLabel();

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
     * @return mixed
     */
    public abstract function property($key);

}
