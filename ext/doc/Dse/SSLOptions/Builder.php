<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\SSLOptions;

/**
 * SSLOptions builder allows fluent configuration of ssl options.
 *
 * @see \Dse::ssl()
 * @see \Dse\Cluster\Builder::withSSL()
 */
final class Builder {

    /**
     * Builds SSL options.
     * @return \Dse\SSLOptions ssl options configured accordingly.
     */
    public function build() { }

    /**
     * Adds a trusted certificate. This is used to verify node's identity.
     *
     * @throws \Dse\Exception\InvalidArgumentException
     *
     * @param string $path ,... one or more paths to files containing a PEM formatted certificate.
     * @return Builder self
     */
    public function withTrustedCerts($path) { }

    /**
     * Disable certificate verification.
     *
     * @throws \Dse\Exception\InvalidArgumentException
     *
     * @param int $flags
     * @return Builder self
     */
    public function withVerifyFlags($flags) { }

    /**
     * Set client-side certificate chain.
     *
     * This is used to authenticate the client on the server-side. This should contain the entire Certificate
     * chain starting with the certificate itself.
     *
     * @throws \Dse\Exception\InvalidArgumentException
     *
     * @param string $path path to a file containing a PEM formatted certificate.
     * @return Builder self
     */
    public function withClientCert($path) { }

    /**
     * Set client-side private key. This is used to authenticate the client on
     * the server-side.
     *
     * @throws \Dse\Exception\InvalidArgumentException
     *
     * @param string $path Path to the private key file
     * @param string|null $passphrase Passphrase for the private key, if any
     * @return Builder self
     */
    public function withPrivateKey($path, $passphrase) { }

}
