<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Symfony\Component\Process\Process;
use Symfony\Component\Process\ProcessBuilder;

/**
 * Embedded ADS for authentication with Cassandra/DSE using Kerberos and LDAP
 */
class EmbeddedAds {
    /**
     * Host/IP address for the ADS server
     */
    const HOST = "127.0.0.1";
    /**
     * Port for the Lightweight Directory Access Protocol (LDAP)
     */
    const LDAP_PORT = "10389";

    /**
     * Embedded ADS Java JAR filename
     */
    const JAR = "embedded-ads.jar";
    /**
     * Configuration directory
     */
    const CONFIGURATION_DIRECTORY = "ads_config";
    /**
     * Configuration filename
     */
    const CONFIGURATION_FILENAME = "krb5.conf";
    /**
     * Cassandra principal keytab filename
     */
    const CASSANDRA_KEYTAB_FILENAME = "cassandra.keytab";
    /**
     * DSE principal keytab filename
     */
    const DSE_KEYTAB_FILENAME = "dse.keytab";
    /**
     * DSEUSER principal keytab filename
     */
    const DSEUSER_KEYTAB_FILENAME = "dseuser.keytab";
    /**
     * Unknown principal keytab filename
     */
    const UNKNOWN_KEYTAB_FILENAME = "unknown.keytab";

    /**
     * Cassandra service principal
     */
    const CASSANDRA_SERVICE_PRINCIPAL = "cassandra@DATASTAX.COM";
    /**
     * DSE service principal
     */
    const DSE_SERVICE_PRINCIPAL = "dse/_HOST@DATASTAX.COM";
    /**
     * DSEUSER service principal
     */
    const DSEUSER_SERVICE_PRINCIPAL = "dseuser@DATASTAX.COM";
    /**
     * Service principal realm
     */
    const REALM = "DATASTAX.COM";

    /**
     * @var Process ADS process
     */
    private $process;
    /**
     * @var string Absolute path to configuration directory
     */
    private $configuration_directory;
    /**
     * @var string Absolute path to configuration file
     */
    private $configuration_file;
    /**
     * @var string Absolute path to Cassandra keytable file
     */
    private $cassandra_keytab_file;
    /**
     * @var string Absolute path to DSE keytable file
     */
    private $dse_keytab_file;
    /**
     * @var string Absolute path to DSE user keytable file
     */
    private $dseuser_keytab_file;
    /**
     * @var string Absolute path to unknown keytable file
     */
    private $unknown_keytab_file;
    /**
     * @var bool True if verbose output should be enable; false otherwise
     */
    private $verbose;

    /**
     * EmbeddedAds constructor
     *
     * @param bool $verbose (Optional) True if verbose output should be enabled;
     *                                 false otherwise (default: false)
     * @throws Exception If execution is requested on Windows platform
     */
    public function __construct($verbose = false) {
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' ||
            strtoupper(substr(PHP_OS, 0, 6)) === 'CYGWIN') {
            throw new Exception("ADS Server will not be Created: "
                . " Must run locally with DSE cluster");
        }
        //TODO: Use SSH support to copy and execute ADS remotely

        // Ensure all application dependencies are available for ADS
        $failures = array();
        if (!$this->is_java_available()) {
            $failures[] = "Java";
        }
        if (!$this->is_kerberos_client_available()) {
            $failures[] = "kinit and kdestroy";
        }

        // Ensure ADS is available
        $ads_jar_filename = __DIR__ . DIRECTORY_SEPARATOR . self::JAR;
        if (!file_exists($ads_jar_filename)) {
            $failures[] = "ADS";
        }

        // Ensure no issues were detected
        if (count($failures) > 0) {
            throw new Exception("Unable to Launch ADS Server: Missing "
                . implode(" and ", $failures));
        }

        // Configure components of the ADS (settings)
        $this->configuration_directory = sys_get_temp_dir() . DIRECTORY_SEPARATOR
            . self::CONFIGURATION_DIRECTORY . DIRECTORY_SEPARATOR;
        $this->configuration_file = $this->configuration_directory
            . self::CONFIGURATION_FILENAME;
        $this->cassandra_keytab_file = $this->configuration_directory
            . self::CASSANDRA_KEYTAB_FILENAME;
        $this->dse_keytab_file = $this->configuration_directory
            . self::DSE_KEYTAB_FILENAME;
        $this->dseuser_keytab_file = $this->configuration_directory
            . self::DSEUSER_KEYTAB_FILENAME;
        $this->unknown_keytab_file = $this->configuration_directory
            . self::UNKNOWN_KEYTAB_FILENAME;
        $this->verbose = $verbose;

        // Create the command for starting the ADS
        $ads_command = array();
        /**
         * Add `exec` to the ADS command to properly terminate process chain.
         *
         * NOTE: Due to some limitations in PHP, if you want to get the pid of a
         *       symfony Process, you may have to prefix your commands with exec.
         *       Please read Symfony Issue #5759 to understand why this is
         *       happening.
         *       https://github.com/symfony/symfony/issues/5759
         */
        $ads_command[] = "exec";
        $ads_command[] = "java";
        $ads_command[] = "-jar";
        $ads_command[] = $ads_jar_filename;
        $ads_command[] = "-k";
        $ads_command[] = "--confdir";
        $ads_command[] = $this->configuration_directory;
        $process_builder = ProcessBuilder::create($ads_command)
            ->setEnv("KRB5_CONFIG", $this->configuration_file);
        $this->process = $process_builder->getProcess();
    }

    public function __destruct() {
        if ($this->process->isRunning()) {
            if ($this->verbose) {
                echo "Terminating ADS Process: {$this->process->getPid()}" . PHP_EOL;
            }
            $this->stop();
        }
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            // Do not allow the process to be accessed
            if ($property == "process") {
                throw new Exception("ADS process is not accessible");
            }
            return $this->$property;
        }
    }

    public function __set($property, $value) {
        if (property_exists($this, $property)) {
            // Allow the verbose option to be overridden
            if ($property == "verbose") {
                $this->$property = $value;
            }
        }
    }

    /**
     * Start the ADS server as a daemon process
     */
    public function start() {
        // Create the configuration directory for ADS
        @mkdir($this->configuration_directory);

        // Start the process and handle process output
        $this->process->start(function ($type, $buffer) {
            if ($this->verbose) {
                if (Process::ERR === $type) {
                    echo "ADS: ERROR: {$buffer}";
                } else {
                    echo "ADS: {$buffer}";
                }
            }
        });

        // Wait for the ADS to be initialized
        foreach ($this->process as $type => $data) {
            if (stripos($data, "principal initialization complete") !== false) {
                break;
            } else {
                usleep(10000); // 100ms
            }
        }
    }

    /**
     * Stop the ADS server daemon and clean up any configuration directories
     */
    public function stop() {
        // Stop the process and remove the temporary directory
        $this->process->stop(0);
        IntegrationTest::remove_path($this->configuration_directory);
    }

    /**
     * Acquire a ticket from the ADS server and place into the cache
     *
     * @param string $principal Principal identity
     * @param string $keytab_file Filename of keytab to use
     */
    public function acquire_ticket($principal, $keytab_file) {
        $kinit_command = array();
        $kinit_command[] = "kinit";
        $kinit_command[] = "-k";
        $kinit_command[] = "-t";
        $kinit_command[] = $keytab_file;
        $kinit_command[] = $principal;
        $this->execute_command($kinit_command);
    }

    /**
     * Destroy all tickets acquired from the ADS server (remove from cache)
     */
    public function destroy_tickets() {
        $kdestroy_command = array();
        $kdestroy_command[] = "kdestroy";
        $kdestroy_command[] = "-A";
        $this->execute_command($kdestroy_command);
    }

    /**
     * Determine if Java is available
     *
     * @return bool True if java is available; false otherwise
     */
    private function is_java_available() {
        $java_available_command = array();
        $java_available_command[] = "java";
        $java_available_command[] = "-help";
        return $this->execute_command($java_available_command)["exit_code"] == 0;
    }

    /**
     * Determine if the Kerberos client is available (e.g. kinit, kdestroy)
     *
     * @return bool True if the Kerberos clinet is available; false otherwise
     */
    private function is_kerberos_client_available() {
        $kerberos_client_available = array();
        $kerberos_client_available[] = "klist";
        $kerberos_client_available[] = "-V";
        return $this->execute_command($kerberos_client_available)["exit_code"] == 0;
    }

    /**
     * Execute a command while applying the KRB5_CONFIG environment variable
     * pointing to the absolute path for the configuration file
     *
     * @param array $command Command to execute
     * @return array Information about the command executed
     *     [
     *         "exit_code"    => (int) Exit code for the executed command
     *         "exit_message" => (string) Message if command had an error
     *         "output"       => (string) Output from the executed command
     *     ]
     */
    private function execute_command(array $command) {
        // Create the process for the command
        $process_builder = ProcessBuilder::create($command)
            ->setEnv("KRB5_CONFIG", $this->configuration_file)
            ->setTimeout(60);
        $process = $process_builder->getProcess();

        // Execute the command and return the output
        if ($this->verbose) {
            echo "ADS: {$process->getCommandLine()}" . PHP_EOL;
        }
        $process->run(function ($type, $buffer) {
            if ($this->verbose) {
                if (Process::ERR === $type) {
                    echo "ADS: ERROR: {$buffer}";
                } else {
                    echo "ADS: {$buffer}";
                }
            }
        });

        // Return information about the process execution
        return array(
            "exit_code" => $process->getExitCode(),
            "exit_message" => $process->getExitCodeText(),
            "output" => $process->getOutput()
        );
    }
}