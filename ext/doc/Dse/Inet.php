<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `inet` datatype
 */
final class Inet implements Value {

    /**
     * Creates a new IPv4 or IPv6 inet address.
     *
     * @param string $address any IPv4 or IPv6 address
     */
    public function __construct($address) { }

    /**
     * Returns the normalized string representation of the address.
     *
     * @return string address
     */
    public function __toString() { }

    /**
     * The type of this inet.
     *
     * @return Type
     */
    public function type() { }

    /**
     * Returns the normalized string representation of the address.
     *
     * @return string address
     */
    public function address() { }

}
