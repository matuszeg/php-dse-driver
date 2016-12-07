<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 */
interface Property {

    /**
     * @return mixed
     */
    public function name();

    /**
     * @return mixed
     */
    public function value();

    /**
     * @return mixed
     */
    public function parent();

}
