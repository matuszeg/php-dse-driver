<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A future returned from `Session::prepareAsync()`
 * This future will resolve with a PreparedStatement or an exception.
 *
 * @see Session::prepareAsync()
 */
final class FuturePreparedStatement implements Future {

    /**
     * Waits for a given future resource to resolve and throws errors if any.
     *
     * @param int|double|null $timeout A timeout in seconds
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @return PreparedStatement A prepared statement
     */
    public function get($timeout) { }

}
