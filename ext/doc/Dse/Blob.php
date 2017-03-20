<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `blob` datatype
 */
final class Blob implements Value {

    /**
     * Creates a new bytes array.
     *
     * @param string $bytes any bytes
     */
    public function __construct($bytes) { }

    /**
     * Returns bytes as a hex string.
     *
     * @return string bytes as hexadecimal string
     */
    public function __toString() { }

    /**
     * The type of this blob.
     *
     * @return Type
     */
    public function type() { }

    /**
     * Returns bytes as a hex string.
     *
     * @return string bytes as hexadecimal string
     */
    public function bytes() { }

    /**
     * Returns bytes as a binary string.
     *
     * @return string bytes as binary string
     */
    public function toBinaryString() { }

}
