<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * This future results is resolved with Rows.
 *
 * @see Session::executeAsync()
 */
final class FutureRows implements Future {

    /**
     * {@inheritDoc}
     *
     * @param int|double|null $timeout A timeout in seconds
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @return Rows|null The result set
     */
    public function get($timeout) { }

}
