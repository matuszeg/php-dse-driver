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
final class DefaultElement implements Element {

    private final function __construct() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function id() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function label() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function properties() { }

    /**
     * {@inheritDoc}
     * @param mixed $key {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function property($key) { }

}
