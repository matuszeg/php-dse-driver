<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * Configuration settings class to read environment variables in order to
 * configure the test harness appropriately. Some properties can be overridden
 * in order to facilitate differing test requirements.
 *
 * Environment variables used for configuration settings:
 *     "DSE"          | If available and true; DSE will be used
 *     "DSE_USERNAME" | Username for DSE download authentication
 *     "DSE_PASSWORD" | Password for DSE download authentication
 *     "PREFIX"       | Cluster prefix to use (default: php-driver)
 *     "VERBOSE"      | If available and true; verbose output will be enabled
 *     "VERSION"      | Cassandra/DSE version to use
 */
class Configuration {
    /**
     * Default Cassandra version to be used
     */
    const DEFAULT_CASSANDRA_VERSION = "3.0.10";
    /**
     * Default DSE version to be used
     */
    const DEFAULT_DSE_VERSION = "5.0.4";

    /**
     * @var bool Flag to determine if DSE is being used
     */
    private $dse = false;
    /**
     * @var string Username for DSE download authentication
     */
    private $dse_username;
    /**
     * @var string Password for DSE download authentication
     */
    private $dse_password;
    /**
     * @var string Cluster prefix
     */
    private $prefix = "php-driver";
    /**
     * @var bool True if verbose output should be enable; false otherwise
     */
    private $verbose = false;
    /**
     * @var string Server version to be  used
     */
    private $version;

    public function __construct() {
        $this->configure();
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            return $this->$property;
        }
    }

    public function __set($property, $value) {
        if (property_exists($this, $property)) {
            // Allow the DSE and version flag to be overridden for features
            if ($property == "dse" || $property == "prefix" || $property == "version") {
                if ($property == "version") {
                    if (!is_a($value, 'Cassandra\Version') &&
                        !is_a($value, 'Dse\Version')) {
                        throw new Exception("Overriding version is not Cassandra or DSE version");
                    }
                }
                $this->$property = $value;
                $this->dse_authentication_credentials();
            }
        }
    }

    /**
     * Configure the settings based on environment or server variables
     */
    private function configure() {
        // Determine if Cassandra or DSE is to be used
        if (isset($_SERVER["DSE"])) {
            $this->dse = boolval($_SERVER["DSE"]);
        }

        // Determine if the DSE download authentication credentials are required
        $this->dse_authentication_credentials();

        // Determine if Cluster prefix is to be overridden
        if (isset($_SERVER["PREFIX"])) {
            $this->prefix = $_SERVER["PREFIX"];
        }

        // Determine the verbose output should be enabled
        if (isset($_SERVER["VERBOSE"])) {
            $this->verbose = boolval($_SERVER["VERBOSE"]);
        }

        // Determine the version of Cassandra or DSE to be used
        if (isset($_SERVER["VERSION"])) {
            $version = $_SERVER["VERSION"];
        } else {
            $version = $this->dse ? self::DEFAULT_DSE_VERSION : self::DEFAULT_CASSANDRA_VERSION;
        }
        if ($this->dse) {
            $this->version = new Dse\Version($version);
        } else {
            $this->version = new Cassandra\Version($version);
        }
    }

    /**
     * Assign the DSE authentication credentials
     *
     * @throws Exception If DSE authentication credentials are not configured
     *                   for CCM cluster creation (e.g. Username/Password are
     *                   missing if configured using username/password
     *                   authentication credentials
     */
    private function dse_authentication_credentials() {
        //TODO Check for INI credentials and local installation directory
        // Determine if the DSE download authentication credentials are required
        if (!isset($this->dse_username) || !isset($this->dse_password)) {
            if ($this->dse) {
                if (isset($_SERVER["DSE_USERNAME"])) {
                    $this->dse_username = $_SERVER["DSE_USERNAME"];
                } else {
                    throw new Exception("DSE Cannot be Enabled: DSE_USERNAME environment variable is unavailable");
                }
                if (isset($_SERVER["DSE_PASSWORD"])) {
                    $this->dse_password = $_SERVER["DSE_PASSWORD"];
                } else {
                    throw new Exception("DSE Cannot be Enabled: DSE_PASSWORD environment variable is unavailable");
                }
            }
        }
    }
}

