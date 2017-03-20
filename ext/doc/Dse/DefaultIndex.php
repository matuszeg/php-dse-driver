<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of an index
 */
final class DefaultIndex implements Index {

    /**
     * {@inheritDoc}
     *
     * @return string Name of the index
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string Kind of the index
     */
    public function kind() { }

    /**
     * {@inheritDoc}
     *
     * @return string Target column name of the index
     */
    public function target() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return Value Value of an option by name
     */
    public function option($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array A dictionary of `string` and `Value` pairs of the
     */
    public function options() { }

    /**
     * {@inheritDoc}
     *
     * @return string Class name of a custom index
     */
    public function className() { }

    /**
     * {@inheritDoc}
     *
     * @return bool
     */
    public function isCustom() { }

}
