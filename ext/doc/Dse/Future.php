<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Futures are returns from asynchronous methods.
 *
 * @see Cluster::connectAsync()
 * @see Session::executeAsync()
 * @see Session::prepareAsync()
 * @see Session::closeAsync()
 */
interface Future {

    /**
     * Waits for a given future resource to resolve and throws errors if any.
     *
     * @param int|double|null $timeout A timeout in seconds
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @return mixed a value that the future has been resolved with
     */
    public function get($timeout);

}
