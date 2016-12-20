<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\RetryPolicy;

/**
 * A retry policy that never retries and allows all errors to fallthrough.
 */
final class Fallthrough implements RetryPolicy {

}
