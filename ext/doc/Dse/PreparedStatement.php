<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Prepared statements are faster to execute because the server doesn't need
 * to process a statement's CQL during the execution.
 *
 * With token-awareness enabled in the driver, prepared statements are even
 * faster, because they are sent directly to replica nodes and avoid the extra
 * network hop.
 *
 * @see Session::prepare()
 */
final class PreparedStatement implements Statement {

    private function __construct() { }

}
