<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A class that contains and manages connections to DSE. It allows for
 * executing or preparing queries for execution.
 *
 * @see Cluster::connect()
 * @see Cluster::connectAsync()
 */
interface Session {

    /**
     * Execute a query.
     *
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options execution options (optional)
     *
     * @throws Exception
     *
     * @return Rows A collection of rows.
     */
    public function execute($statement, $options);

    /**
     * Execute a query asynchronously. This method returns immediately, but
     * the query continues execution in the background.
     *
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options execution options (optional)
     *
     * @return FutureRows A future that can be used to retrieve the result.
     */
    public function executeAsync($statement, $options);

    /**
     * Prepare a query for execution.
     *
     * @param string $cql The query to be prepared.
     * @param ExecutionOptions $options Options to control preparing the query.
     *
     * @throws Exception
     *
     * @return PreparedStatement A prepared statement that can be bound with parameters and executed.
     */
    public function prepare($cql, $options);

    /**
     * Asynchronously prepare a query for execution.
     *
     * @param string $cql The query to be prepared.
     * @param ExecutionOptions $options Options to control preparing the query.
     *
     * @return FuturePreparedStatement A future that can be used to retrieve the prepared statement.
     */
    public function prepareAsync($cql, $options);

    /**
     * Close the session and all its connections.
     *
     * @param double $timeout The amount of time in seconds to wait for the session to close.
     *
     * @throws Exception
     *
     * @return null Nothing.
     */
    public function close($timeout);

    /**
     * Asynchronously close the session and all its connections.
     *
     * @return FutureClose A future that can be waited on.
     */
    public function closeAsync();

    /**
     * Get a snapshot of the cluster's current schema.
     *
     * @return Schema A snapshot of the cluster's schema.
     */
    public function schema();

    /**
     * Execute graph queries.
     *
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     *
     *                       * array["graph_language"]     string       Graph language; default "gremlin-groovy"
     *                       * array["graph_source"]       string       Graph source; default "g". If running
     *                                                                  analytics (OLAP) query then it should
     *                                                                  use "a"
     *                       * array["graph_name"]         string       Graph name
     *                       * array["read_consistency"]   consistency  Read consistency of graph queries;
     *                                                                  default `Dse::CONSISTENCY_ONE`
     *                       * array["write_consistency"]  consistency  Write consistency of graph queries;
     *                                                                  default `Dse::CONSISTENCY_QUORUM`
     *                       * array["request_timeout"]    double|int   Request time of graph queries in
     *                                                                  seconds; default `0` (no timeout)
     *                       * array["timestamp"]          int|string   Either an integer or integer string
     *                                                                  timestamp that represents the number
     *                                                                  of microseconds since the epoch
     *
     * @throws Exception
     *
     * @return Graph\ResultSet A result set.
     */
    public function executeGraph($statement, $options);

    /**
     * Asynchronously execute a graph query.
     *
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     *                       @see Session::executeGraph()
     *
     * @return Graph\FutureResultSet A future that can be used to retrieve the result set.
     */
    public function executeGraphAsync($statement, $options);

}
