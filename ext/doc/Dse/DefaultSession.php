<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * {@inheritDoc}
 */
final class DefaultSession implements Session {

    /**
     * {@inheritDoc}
     * @param mixed $statement {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function execute($statement, $options) { }

    /**
     * {@inheritDoc}
     * @param mixed $statement {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function executeAsync($statement, $options) { }

    /**
     * {@inheritDoc}
     * @param mixed $cql {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function prepare($cql, $options) { }

    /**
     * {@inheritDoc}
     * @param mixed $cql {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function prepareAsync($cql, $options) { }

    /**
     * {@inheritDoc}
     * @param mixed $timeout {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function close($timeout) { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function closeAsync() { }

    /**
     * {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function schema() { }

    /**
     * {@inheritDoc}
     * @param mixed $statement {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function executeGraph($statement, $options) { }

    /**
     * {@inheritDoc}
     * @param mixed $statement {@inheritDoc}
     * @param mixed $options {@inheritDoc}
     * @return mixed {@inheritDoc}
     */
    public function executeGraphAsync($statement, $options) { }

}
