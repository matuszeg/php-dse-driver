<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A future that resolves with Session.
 *
 * @see Cluster::connectAsync()
 */
final class FutureSession implements Future {

    /**
     * {@inheritDoc}
     *
     * @param int|double|null $timeout A timeout in seconds
     *
     * @throws Exception\InvalidArgumentException
     * @throws Exception\TimeoutException
     *
     * @return Session A connected session
     */
    public function get($timeout) { }

}
