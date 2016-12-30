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
interface Index {

    /**
     * Returns the name of the index
     * @return string Name of the index
     */
    public function name();

    /**
     * Returns the kind of index
     * @return string Kind of the index
     */
    public function kind();

    /**
     * Returns the target column of the index
     * @return string Target column name of the index
     */
    public function target();

    /**
     * Return a column's option by name
     * @param mixed $name
     * @return Cassandra\Value Value of an option by name
     */
    public function option($name);

    /**
     * Returns all the index's options
     *               index's options.
     * @return array A dictionary of `string` and `Cassandra\Value pairs of the
     */
    public function options();

    /**
     * Returns the class name of the index
     * @return string Class name of a custom index
     */
    public function className();

    /**
     * Returns `true` if this is a custom index
     * @return boolean
     */
    public function isCustom();

}