<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Batch statements are used to execute a series of simple or prepared
 * statements.
 *
 * There are 3 types of batch statements:
 *  * `Cassandra::BATCH_LOGGED`   - this is the default batch type. This batch
 *    guarantees that either all or none of its statements will be executed.
 *    This behavior is achieved by writing a batch log on the coordinator,
 *    which slows down the execution somewhat.
 *  * `Cassandra::BATCH_UNLOGGED` - this batch will not be verified when
 *    executed, which makes it faster than a `LOGGED` batch, but means that
 *    some of its statements might fail, while others - succeed.
 *  * `Cassandra::BATCH_COUNTER`  - this batch is used for counter updates,
 *    which are, unlike other writes, not idempotent.
 *
 * @see Cassandra::BATCH_LOGGED
 * @see Cassandra::BATCH_UNLOGGED
 * @see Cassandra::BATCH_COUNTER
 */
final class BatchStatement implements Statement {

    /**
     * Creates a new batch statement.
     * @param int $type must be one of Cassandra::BATCH_* (default: Cassandra::BATCH_LOGGED).
     */
    public function __construct($type) { }

    /**
     * Adds a statement to this batch.
     *
     *
     * @throws Exception\InvalidArgumentException
     * @param Cassandra\Statement $statement the statement to add
     * @param array $arguments positional or named arguments
     * @return BatchStatement self
     */
    public function add($statement, $arguments) { }

}
