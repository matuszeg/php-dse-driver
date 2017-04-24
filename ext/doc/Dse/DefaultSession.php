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
final class DefaultSession implements Session {

    /**
     * Execute a query.
     *
     * Available execution options:
     * | Option Name        | Option **Type** | Option Details                                                                                           |
     * |--------------------|-----------------|----------------------------------------------------------------------------------------------------------|
     * | arguments          | array           | An array or positional or named arguments                                                                |
     * | consistency        | int             | A consistency constant e.g Dse::CONSISTENCY_ONE, Dse::CONSISTENCY_QUORUM, etc.                           |
     * | timeout            | int             | A number of rows to include in result for paging                                                         |
     * | paging_state_token | string          | A string token use to resume from the state of a previous result set                                     |
     * | retry_policy       | RetryPolicy     | A retry policy that is used to handle server-side failures for this request                              |
     * | serial_consistency | int             | Either Dse::CONSISTENCY_SERIAL or Dse::CONSISTENCY_LOCAL_SERIAL                                          |
     * | timestamp          | int\|string     | Either an integer or integer string timestamp that represents the number of microseconds since the epoch |
     * | execute_as         | string          | User to execute statement as                                                                             |
     *
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options Options to control execution of the query.
     *
     * @throws Exception
     *
     * @return Rows A collection of rows.
     */
    public function execute($statement, $options) { }

    /**
     * Execute a query asynchronously. This method returns immediately, but
     * the query continues execution in the background.
     *
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options Options to control execution of the query.
     *
     * @return FutureRows A future that can be used to retrieve the result.
     *
     * @see Session::execute() for valid execution options
     */
    public function executeAsync($statement, $options) { }

    /**
     * Prepare a query for execution.
     *
     * @param string $cql The query to be prepared.
     * @param array|ExecutionOptions|null $options Options to control preparing the query.
     *
     * @throws Exception
     *
     * @return PreparedStatement A prepared statement that can be bound with parameters and executed.
     *
     * @see Session::execute() for valid execution options
     */
    public function prepare($cql, $options) { }

    /**
     * Asynchronously prepare a query for execution.
     *
     * @param string $cql The query to be prepared.
     * @param array|ExecutionOptions|null $options Options to control preparing the query.
     *
     * @return FuturePreparedStatement A future that can be used to retrieve the prepared statement.
     *
     * @see Session::execute() for valid execution options
     */
    public function prepareAsync($cql, $options) { }

    /**
     * Close the session and all its connections.
     *
     * @param double $timeout The amount of time in seconds to wait for the session to close.
     *
     * @throws Exception
     *
     * @return null Nothing.
     */
    public function close($timeout) { }

    /**
     * Asynchronously close the session and all its connections.
     *
     * @return FutureClose A future that can be waited on.
     */
    public function closeAsync() { }

    /**
     * Get a snapshot of the cluster's current schema.
     *
     * @return Schema A snapshot of the cluster's schema.
     */
    public function schema() { }

    /**
     * Execute graph queries.
     *
     * Available execution options:
     * | Option Name        | Option **Type** | Option Details                                                                                           |
     * |--------------------|-----------------|----------------------------------------------------------------------------------------------------------|
     * | graph_language     | string          | Graph language; default "gremlin-groovy"                                                                 |
     * | graph_source       | string          | Graph source; default "g". If running analytics (OLAP) query then it should use "a"                      |
     * | graph_name         | string          | Graph name                                                                                               |
     * | read_consistency   | consistency     | Read consistency of graph queries; default Dse::CONSISTENCY_ONE                                          |
     * | write_consistency  | consistency     | Write consistency of graph queries; default Dse::CONSISTENCY_QUORUM                                      |
     * | request_timeout    | double\|int     | Request time of graph queries in seconds; default `0` (no timeout)                                       |
     * | timestamp          | int\|string     | Either an integer or integer string timestamp that represents the number of microseconds since the epoch |
     *
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     *
     * @throws Exception
     *
     * @return Graph\ResultSet A result set.
     */
    public function executeGraph($statement, $options) { }

    /**
     * Asynchronously execute a graph query.
     *
     * @param string|Graph\Statement $statement A graph statement or query string to be executed.
     * @param array $options Options to control execution of the graph query.
     *
     * @return Graph\FutureResultSet A future that can be used to retrieve the result set.
     *
     * @see Session::executeGraph() for valid execution options
     */
    public function executeGraphAsync($statement, $options) { }

}
