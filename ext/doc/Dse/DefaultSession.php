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
     *
     * @param string|Statement $statement {@inheritDoc}
     * @param array|ExecutionOptions|null $options {@inheritDoc}
     *
     * @return Rows {@inheritDoc}
     */
    public function execute($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string|Statement $statement {@inheritDoc}
     * @param array|ExecutionOptions|null $options {@inheritDoc}
     *
     * @return FutureRows {@inheritDoc}
     */
    public function executeAsync($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string $cql {@inheritDoc}
     * @param ExecutionOptions $options {@inheritDoc}
     *
     * @return PreparedStatement {@inheritDoc}
     */
    public function prepare($cql, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string $cql {@inheritDoc}
     * @param ExecutionOptions $options {@inheritDoc}
     *
     * @return FuturePreparedStatement {@inheritDoc}
     */
    public function prepareAsync($cql, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param double $timeout {@inheritDoc}
     *
     * @return null {@inheritDoc}
     */
    public function close($timeout) { }

    /**
     * {@inheritDoc}
     *
     * @return FutureClose {@inheritDoc}
     */
    public function closeAsync() { }

    /**
     * {@inheritDoc}
     *
     * @return Schema {@inheritDoc}
     */
    public function schema() { }

    /**
     * {@inheritDoc}
     *
     * @param string|Graph\Statement $statement {@inheritDoc}
     * @param array $options {@inheritDoc}
     *
     * @return Graph\ResultSet {@inheritDoc}
     */
    public function executeGraph($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string|Graph\Statement $statement {@inheritDoc}
     * @param array $options {@inheritDoc}
     *
     * @return Graph\FutureResultSet {@inheritDoc}
     */
    public function executeGraphAsync($statement, $options) { }

}
