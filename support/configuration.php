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
 *     "DSE"             | If available and true; DSE will be used
 *     "DSE_INI"         | If available and true; DSE INI credentials will be
 *                       | used
 *                       | NOTE: If available, SSH connection is in use and is
 *                       |       pointing to a valid filename; the file will be
 *                       |       copied to remote host and used for DSE
 *                       |       download authentication
 *     "DSE_USERNAME"    | Username for DSE download authentication
 *     "DSE_PASSWORD"    | Password for DSE download authentication
 *     "KEEP_CLUSTERS"   | If available and true; CCM clusters will not be
 *                       | removed on startup and shutdown
 *     "PREFIX"          | Cluster prefix to use (default: php-driver)
 *     "SSH_HOST"        | If available; IPv4 to use for SSH connection
 *                       | (default: disabled)
 *     "SSH_HOSTNAME"    | If available; DNS hostname to use for SSH connection
 *                       | (default: disabled)
 *     "SSH_PASSWORD"    | Password to use for username/password authentication
 *                       | or for private key passphrase using public key
 *                       | authentication (default: vagrant for
 *                       | username/password authentication, NULL for private
 *                       | key passphrase)
 *     "SSH_PORT"        | Port to use for SSH connection (default: 22)
 *     "SSH_PRIVATE_KEY" | If available; private key to use for SSH connection
 *                       | NOTE: Use SSH_PASSWORD if passphrase is required for
 *                       |       private key
 *     "SSH_PUBLIC_KEY"  | If available; public key to use for SSH connection
 *     "SSH_USERNAME"    | Username to use for username/password or public key
 *                       | authentication (default: vagrant)
 *     "UNSTABLE"        | If available and true; unstable tests will be
 *                       | executed
 *     "VERBOSE"         | If available and true; verbose output will be
 *                       | enabled
 *     "VERSION"         | Cassandra/DSE version to use
 */
class Configuration {
    /**
     * Default Cassandra version to be used
     */
    const DEFAULT_CASSANDRA_VERSION = "3.0.10";
    /**
     * Default DSE version to be used
     */
    const DEFAULT_DSE_VERSION = "5.0.5";

    /**
     * @var bool Flag to determine if DSE is being used
     */
    private $dse = false;
    /**
     * @var bool|string DSE ini filename if using SSH deployments; true to
     *                  utilize local INI credentials file
     *                  NOTE: For SSH deployments INI credentials file will be
     *                        checked to make sure it exists and copied over to
     *                        SSH deployment for use
     */
    private $dse_ini = false;
    /**
     * @var string Username for DSE download authentication
     */
    private $dse_username;
    /**
     * @var string Password for DSE download authentication
     */
    private $dse_password;
    /**
     * @var bool Flag to determine if CCM clusters should be removed during
     *           startup and shutdown of test harness
     */
    private $keep_clusters = false;
    /**
     * @var string Cluster prefix
     */
    private $prefix = "php-driver";
    /**
     * @var bool|string IPv4 address to use for SSH deployments; false if not
     *                  utilized
     */
    private $ssh_host = false;
    /**
     * @var bool|string DNS hostname to use for SSH deployments; false if not
     *                  utilized
     */
    private $ssh_hostname = false;
    /**
     * @var string Password to use for SSH deployments
     */
    private $ssh_password = "vagrant";
    /**
     * @var bool|string Location of private key file
     */
    private $ssh_private_key = false;
    /**
     * @var bool|string Location of public key file
     */
    private $ssh_public_key = false;
    /**
     * @var int Port to use for SSH deployments
     */
    private $ssh_port = 22;
    /**
     * @var string Username to use for SSH deployments
     */
    private $ssh_username = "vagrant";
    /**
     * @var bool Flag to determine if unstable tests should run
     */
    private $unstable = false;
    /**
     * @var bool True if verbose output should be enable; false otherwise
     */
    private $verbose = false;
    /**
     * @var Cassandra\Version|Dse\Version Server version to be used
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
            if ($property == "dse" || $property == "prefix" || $property == "version" ||
                $property == "verbose") {
                if ($property == "version") {
                    if (!($value instanceof Cassandra\Version) &&
                        !($value instanceof Dse\Version)) {
                        throw new Exception("Overriding version is not Cassandra or DSE version");
                    }
                }
                $this->$property = $value;
                $this->dse_authentication_credentials();
            }
        }
    }

    /**
     * Display the current configuration settings
     */
    public function print_settings() {
        if ($this->dse) {
            echo "  DSE Version: v{$this->version}/v{$this->version->cassandra_version}" . PHP_EOL;
            if ($this->dse_ini) {
                $dse_ini = $this->dse_ini;
                if (is_bool($dse_ini)) {
                    $dse_ini = $dse_ini ? "True" : "False";
                }
                echo "    INI Credentials: {$dse_ini}" . PHP_EOL;
            } else {
                echo "    Username: {$this->dse_username}" . PHP_EOL
                . "    Password: {$this->dse_password}" . PHP_EOL;
            }
        } else {
            echo "  Cassandra Version: {$this->version}" . PHP_EOL;
        }
        echo "  Deployment: ";
        if ($this->ssh_host) {
            echo "SSH" . PHP_EOL
                . "    Host: {$this->ssh_host}:{$this->ssh_port}" . PHP_EOL;
                if ($this->ssh_hostname) {
                    echo "    Hostname: {$this->ssh_hostname}" . PHP_EOL;
                }
                echo "    Username: {$this->ssh_username}";
            if (!is_null($this->ssh_password)) {
                echo PHP_EOL . "    Password: {$this->ssh_password}";
            }
            if ($this->ssh_public_key && $this->ssh_private_key) {
                echo PHP_EOL . "    Public Key: {$this->ssh_public_key}" . PHP_EOL
                    . "    Private Key: {$this->ssh_private_key}";
            }
        } else {
            echo "Local";
        }
        echo PHP_EOL . "  CCM Cluster Prefix: {$this->prefix}" . PHP_EOL
            . "  Executing Unstable Tests: " . ($this->unstable ? "True" : "False") . PHP_EOL
            . "  Keep CCM Clusters: " . ($this->keep_clusters ? "True" : "False") . PHP_EOL;
    }

    /**
     * Configure the settings based on environment or server variables
     */
    private function configure() {
        // Determine if Cassandra or DSE is to be used
        if (isset($_SERVER["DSE"])) {
            $this->dse = filter_var($_SERVER["DSE"], FILTER_VALIDATE_BOOLEAN);
        }

        // Determine if clusters startup/shutdown settings is to be overridden
        if (isset($_SERVER["KEEP_CLUSTERS"])) {
            $this->keep_clusters = filter_var($_SERVER["KEEP_CLUSTERS"], FILTER_VALIDATE_BOOLEAN);
        }

        // Determine if cluster prefix is to be overridden
        if (isset($_SERVER["PREFIX"])) {
            $this->prefix = $_SERVER["PREFIX"];
        }

        // Determine if SSH deployment settings are to be overridden
        if (isset($_SERVER["SSH_HOST"])) {
            $this->ssh_host = filter_var($_SERVER["SSH_HOST"], FILTER_VALIDATE_IP);
        }
        if (isset($_SERVER["SSH_HOSTNAME"])) {
            $this->ssh_hostname = $_SERVER["SSH_HOSTNAME"];
        }
        if (isset($_SERVER["SSH_PUBLIC_KEY"]) &&
            isset($_SERVER["SSH_PRIVATE_KEY"])) {
            $this->ssh_public_key = $_SERVER["SSH_PUBLIC_KEY"];
            $this->ssh_private_key = $_SERVER["SSH_PRIVATE_KEY"];
            if (!isset($_SERVER["SSH_PASSWORD"])) {
                $this->ssh_password = null;
            }
        }
        if (isset($_SERVER["SSH_PASSWORD"])) {
            $this->ssh_password = $_SERVER["SSH_PASSWORD"];
        }
        if (isset($_SERVER["SSH_PORT"])) {
            $this->ssh_port = $_SERVER["SSH_PORT"];
        }
        if (isset($_SERVER["SSH_USERNAME"])) {
            $this->ssh_username = $_SERVER["SSH_USERNAME"];
        }

        // Determine if the DSE download authentication credentials are required
        $this->dse_authentication_credentials();

        // Determine if unstable tests should be run
        if (isset($_SERVER["UNSTABLE"])) {
            $this->unstable = filter_var($_SERVER["UNSTABLE"], FILTER_VALIDATE_BOOLEAN);
        }

        // Determine the verbose output should be enabled
        if (isset($_SERVER["VERBOSE"])) {
            $this->verbose = filter_var($_SERVER["VERBOSE"], FILTER_VALIDATE_BOOLEAN);
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
        //TODO Check for local installation directory
        // Determine if the DSE INI download authentication credentials are in use
        if (isset($_SERVER["DSE_INI"])) {
            // Check to see if a valid true boolean is set (validate filename)
            $this->dse_ini = filter_var($_SERVER["DSE_INI"], FILTER_VALIDATE_BOOLEAN);
            if (!$this->dse_ini && $this->ssh_host) {
                $this->dse_ini = $_SERVER["DSE_INI"];
                if (!file_exists($this->dse_ini)) {
                    throw new Exception("DSE Cannot be Enabled: DSE_INI environment variable points to invalid file");
                }
            }
        }

        // Determine if the DSE username/password download authentication credentials are in use
        if (!$this->dse_ini &&
            (!isset($this->dse_username) || !isset($this->dse_password))) {
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

