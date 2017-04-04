<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Batch statements are used to execute a series of simple or prepared
 * statements.
 *
 * There are 3 types of batch statements:
 *  * `Dse::BATCH_LOGGED`   - this is the default batch type. This batch
 *    guarantees that either all or none of its statements will be executed.
 *    This behavior is achieved by writing a batch log on the coordinator,
 *    which slows down the execution somewhat.
 *  * `Dse::BATCH_UNLOGGED` - this batch will not be verified when
 *    executed, which makes it faster than a `LOGGED` batch, but means that
 *    some of its statements might fail, while others - succeed.
 *  * `Dse::BATCH_COUNTER`  - this batch is used for counter updates,
 *    which are, unlike other writes, not idempotent.
 *
 * @see Dse::BATCH_LOGGED
 * @see Dse::BATCH_UNLOGGED
 * @see Dse::BATCH_COUNTER
 */
final class BatchStatement implements Statement {

    /**
     * Creates a new batch statement.
     *
     * @param int $type must be one of Dse::BATCH_* (default: Dse::BATCH_LOGGED).
     */
    public function __construct($type) { }

    /**
     * Adds a statement to this batch.
     *
     * @param string|Statement $statement string or statement to add
     * @param array|null $arguments positional or named arguments (optional)
     *
     * @throws Exception\InvalidArgumentException
     *
     * @return BatchStatement self
     */
    public function add($statement, $arguments) { }

}
