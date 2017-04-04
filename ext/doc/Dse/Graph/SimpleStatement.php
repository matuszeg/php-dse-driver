<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * A simple string base graph query.
 */
final class SimpleStatement implements Statement {

    /**
     * Construct a graph statement with a string query.
     *
     * @param string $query The graph query.
     */
    public function __construct($query) { }

}
