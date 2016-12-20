<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * An interface implemented by all exceptions thrown by the PHP Driver.
 * Makes it easy to catch all driver-related exceptions using
 * `catch (Cassandra\Exception $e)`.
 */
interface Exception {

}
