<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `uuid` datatype
 */
final class Uuid implements Value, UuidInterface {

    /**
     * Creates a uuid from a given uuid string or a random one.
     * @param string $uuid A uuid string
     */
    public function __construct($uuid) { }

    /**
     * Returns this uuid as string.
     * @return string uuid
     */
    public function __toString() { }

    /**
     * The type of this uuid.
     * @return Type
     */
    public function type() { }

    /**
     * Returns this uuid as string.
     * @return string uuid
     */
    public function uuid() { }

    /**
     * Returns the version of this uuid.
     * @return int version of this uuid
     */
    public function version() { }

}
