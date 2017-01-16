<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * {@inheritDoc}
 */
final class DefaultProperty implements Property {

    private final function __construct() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function name() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function value() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function parent() { }

}
