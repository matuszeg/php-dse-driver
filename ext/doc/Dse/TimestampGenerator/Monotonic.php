<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\TimestampGenerator;

/**
 * A timestamp generator that generates monotonically increasing timestamps
 * client-side. The timestamps generated have a microsecond granularity with
 * the sub-millisecond part generated using a counter. The implementation
 * guarantees that no more than 1000 timestamps will be generated for a given
 * clock tick even if shared by multiple session objects. If that rate is
 * exceeded then a warning is logged and timestamps stop incrementing until
 * the next clock tick.
 */
final class Monotonic implements TimestampGenerator {

}
