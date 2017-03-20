<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a column
 */
final class DefaultColumn implements Column {

    /**
     * {@inheritDoc}
     *
     * @return string Name of the column or null
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return Type Type of the column
     */
    public function type() { }

    /**
     * {@inheritDoc}
     *
     * @return bool Whether the column is stored in descending order.
     */
    public function isReversed() { }

    /**
     * {@inheritDoc}
     *
     * @return bool Whether the column is static
     */
    public function isStatic() { }

    /**
     * {@inheritDoc}
     *
     * @return bool Whether the column is frozen
     */
    public function isFrozen() { }

    /**
     * {@inheritDoc}
     *
     * @return string Name of the index if defined or null
     */
    public function indexName() { }

    /**
     * {@inheritDoc}
     *
     * @return string Index options if present or null
     */
    public function indexOptions() { }

}
