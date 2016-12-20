<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * This future results is resolved with `Cassandra\Rows`.
 *
 * @see Session::executeAsync()
 */
final class FutureRows implements Future {

    /**
     * {@inheritDoc}
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @param float|null $timeout
     * @return mixed a value that the future has been resolved with
     */
    public function get($timeout) { }

}
