<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Common interface implemented by all Cassandra value types.
 *
 * @see Bigint
 * @see Smallint
 * @see Tinyint
 * @see Blob
 * @see Collection
 * @see Float
 * @see Inet
 * @see Map
 * @see Set
 * @see Timestamp
 * @see Timeuuid
 * @see Uuid
 * @see Varint
 * @see Date
 * @see Time
 *
 * @see Numeric
 * @see UuidInterface
 */
interface Value {

    /**
     * The type of represented by the value.
     * @return Type
     */
    public function type();

}
