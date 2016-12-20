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
interface UuidInterface {

    /**
     * Returns this uuid as string.
     * @return string uuid as string
     */
    public function uuid();

    /**
     * Returns the version of this uuid.
     * @return int version of this uuid
     */
    public function version();

}
