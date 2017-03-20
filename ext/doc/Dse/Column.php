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
interface Column {

    /**
     * Returns the name of the column.
     *
     * @return string Name of the column or null
     */
    public function name();

    /**
     * Returns the type of the column.
     *
     * @return Type Type of the column
     */
    public function type();

    /**
     * Returns whether the column is in descending or ascending order.
     *
     * @return bool Whether the column is stored in descending order.
     */
    public function isReversed();

    /**
     * Returns true for static columns.
     *
     * @return bool Whether the column is static
     */
    public function isStatic();

    /**
     * Returns true for frozen columns.
     *
     * @return bool Whether the column is frozen
     */
    public function isFrozen();

    /**
     * Returns name of the index if defined.
     *
     * @return string Name of the index if defined or null
     */
    public function indexName();

    /**
     * Returns index options if present.
     *
     * @return string Index options if present or null
     */
    public function indexOptions();

}
