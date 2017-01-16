<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A class that contains and manages connections to DSE. It allows for executing or preparing queries for execution.
 */
interface Session {

    /**
     * Execute a query.
     *
     * @throws Exeception
     *
     * @param Dse\Statement $statement A statement to be executed.
     * @param Dse\ExecutionOptions $options Options to control execution of the query.
     * @return Rows A colleciton of rows.
     */
    public function execute($statement, $options);

    /**
     * Execute a query asyncrhonously. This method returns immediately, but
     * the query continues execution in the background.
     * @param Dse\Statement $statement A statement to be executed.
     * @param Dse\ExecutionOptions $options Options to control execution of the query.
     * @return FutureRows A future that can be used to retrieve the result.
     */
    public function executeAsync($statement, $options);

    /**
     * Prepare a query for execution.
     *
     * @throws Exeception
     *
     * @param string $cql The query to be prepared.
     * @param Dse\ExecutionOptions $options Options to control preparing the query.
     * @return PreparedStatement A prepared statement that can be bound with parameters and executed.
     */
    public function prepare($cql, $options);

    /**
     * Asynchronously prepare a query for execution.
     * @param string $cql The query to be prepared.
     * @param Dse\ExecutionOptions $options Options to control preparing the query.
     * @return FuturePreparedStatement A future that can be used to retrieve the prepared statement.
     */
    public function prepareAsync($cql, $options);

    /**
     * Close the session and all its connections.
     *
     * @throws Exeception
     *
     * @param double $timeout The amount of time in seconds to wait for the session to close.
     * @return null Nothing.
     */
    public function close($timeout);

    /**
     * Asynchronously close the session and all its connections.
     * @return FutureClose A future that can be waited on.
     */
    public function closeAsync();

    /**
     * Get a snapshot of the cluster's current schema.
     * @return Schema A snapshot of the cluster's schema.
     */
    public function schema();

    /**
     * Execute graph queries.
     * @throws Exeception
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     * @return Graph\ResultSet A result set.
     */
    public function executeGraph($statement, $options);

    /**
     * Asynchronously execute a graph query.
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     * @return Graph\FutureResultSet A future that can be used to retrieve the result set.
     */
    public function executeGraphAsync($statement, $options);

}
