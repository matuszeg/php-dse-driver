<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Simple statements can be executed using a Session instance.
 * They are constructed with a CQL string that can contain positional
 * argument markers `?`.
 *
 * NOTE: Positional argument are only valid for native protocol v2+.
 *
 * @see Session::execute()
 */
final class SimpleStatement implements Statement {

    /**
     * Creates a new simple statement with the provided CQL.
     * @param string $cql CQL string for this simple statement
     */
    public function __construct($cql) { }

}
