<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph;

/**
 * {@inheritDoc}
 */
final class DefaultVertexProperty extends VertexProperty  {

    private final function __construct() { }

    /**
     * {@inheritDoc}
     *
     * @return string {@inheritDoc}
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return Graph\Result {@inheritDoc}
     */
    public function value() { }

    /**
     * {@inheritDoc}
     *
     * @return Graph\Result {@inheritDoc}
     */
    public function parent() { }

    /**
     * {@inheritDoc}
     *
     * @return Graph\Result {@inheritDoc}
     */
    public function id() { }

    /**
     * {@inheritDoc}
     *
     * @return string {@inheritDoc}
     */
    public function label() { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function properties() { }

    /**
     * {@inheritDoc}
     *
     * @param string $key {@inheritDoc}
     *
     * @return Graph\Result {@inheritDoc}
     */
    public function property($key) { }

}
