<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\RetryPolicy;

/**
 * The default retry policy. This policy retries a query, using the
 * request's original consistency level, in the following cases:
 *
 * * On a read timeout, if enough replicas replied but the data was not received.
 * * On a write timeout, if a timeout occurs while writing a distributed batch log.
 * * On unavailable, it will move to the next host.
 *
 * In all other cases the error will be returned.
 */
final class DefaultPolicy implements RetryPolicy {

}
