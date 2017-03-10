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
     * @var array ADS command
     */
    private $ads_command = array();
    /**
     * @var Process|resource ADS process or SSH stream resource
     */
    private $process;
    /**
     * @var int|string Process ID (PID) of the executing ADS instance
     */
    private $process_id;
    /**
     * @var array(string) Absolute path to configuration directory
     *                    (for clients; this may be the same as the server)
     */
    private $configuration_directory = array();
    /**
     * @var array(string) Absolute path to configuration file
     *                    (for clients; this may be the same as the server)
     */
    private $configuration_file = array();
    /**
     * @var array(string) Absolute path to Cassandra keytab file
     *                    (for clients; this may be the same as the server)
     */
    private $cassandra_keytab_file = array();
    /**
     * @var array(string) Absolute path to DSE keytab file
     *                    (for clients; this may be the same as the server)
     */
    private $dse_keytab_file = array();
    /**
     * @var array(string) Absolute path to DSE user keytab file
     *                    (for clients; this may be the same as the server)
     */
    private $dseuser_keytab_file = array();
    /**
     * @var array(string) Absolute path to unknown keytab file
     *                    (for clients; this may be the same as the server)
     */
    private $unknown_keytab_file = array();
    /**
     * @var resource SSH session for remote deployment
     */
    private $ssh = null;
    /**
     * @var string Hostname entry expected by the remote machine for Kerberos
     *             authentication
     */
    private $ssh_hostname = "ccm-cluster";
    /**
     * @var string Host/IPv4 address for ADS
     */
    private $host = "127.0.0.1";
    /**
     * @var bool True if verbose output should be enable; false otherwise
     */
    private $verbose = false;

    /**
     * EmbeddedAds constructor
     *
     * @param \Configuration|null $configuration (Optional) Configuration settings
     *                                                      to apply to ADS
     * @throws Exception If execution is requested on Windows platform
     */
    public function __construct($configuration = null) {
        // Process the configuration
        $this->process_configuration($configuration);

        // Determine if the ADS can be executed
        if (is_null($this->ssh) &&
            strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' ||
            strtoupper(substr(PHP_OS, 0, 6)) === 'CYGWIN') {
            throw new Exception("ADS Server will not be Created: "
                . " Must run locally with DSE cluster; use SSH");
        }

        // Get the temporary directory and directory separator to use
        $temporary_directory = $this->temporary_directory();
        $directory_separator = $this->directory_separator();

        // Configure components of the ADS (settings)
        $configuration_directory = $temporary_directory . $directory_separator
            . self::CONFIGURATION_DIRECTORY . $directory_separator;
        $this->configuration_directory["server"] = $configuration_directory;
        $this->configuration_file["server"] = $configuration_directory
            . self::CONFIGURATION_FILENAME;
        $this->cassandra_keytab_file["server"] = $configuration_directory
            . self::CASSANDRA_KEYTAB_FILENAME;
        $this->dse_keytab_file["server"] = $configuration_directory
            . self::DSE_KEYTAB_FILENAME;
        $this->dseuser_keytab_file["server"] = $configuration_directory
            . self::DSEUSER_KEYTAB_FILENAME;
        $this->unknown_keytab_file["server"] = $configuration_directory
            . self::UNKNOWN_KEYTAB_FILENAME;

        // Configure the client setting (NOTE: They may be the same as the server)
        if (!is_null($this->ssh)) {
            $configuration_directory = sys_get_temp_dir() . DIRECTORY_SEPARATOR
                . self::CONFIGURATION_DIRECTORY . DIRECTORY_SEPARATOR;
            $this->configuration_directory["client"] = $configuration_directory;
            $this->configuration_file["client"] = $configuration_directory
                . self::CONFIGURATION_FILENAME;
            $this->cassandra_keytab_file["client"] = $configuration_directory
                . self::CASSANDRA_KEYTAB_FILENAME;
            $this->dse_keytab_file["client"] = $configuration_directory
                . self::DSE_KEYTAB_FILENAME;
            $this->dseuser_keytab_file["client"] = $configuration_directory
                . self::DSEUSER_KEYTAB_FILENAME;
            $this->unknown_keytab_file["client"] = $configuration_directory
                . self::UNKNOWN_KEYTAB_FILENAME;
        } else {
            $this->configuration_directory["client"] = $this->configuration_directory["server"];
            $this->configuration_file["client"] = $this->configuration_file["server"];
            $this->cassandra_keytab_file["client"] = $this->cassandra_keytab_file["server"];
            $this->dse_keytab_file["client"] = $this->dse_keytab_file["server"];
            $this->dseuser_keytab_file["client"] = $this->dseuser_keytab_file["server"];
            $this->unknown_keytab_file["client"] = $this->unknown_keytab_file["server"];
        }

        // Ensure all application dependencies are available for ADS
        $failures = array();
        if (!$this->is_java_available()) {
            $failures[] = "Java";
        }
        if (!$this->is_kerberos_client_available()) {
            $failures[] = "kinit and kdestroy";
        }

        // Ensure ADS is available
        $ads_jar_path_and_filename = __DIR__ . DIRECTORY_SEPARATOR . self::JAR;
        if (!file_exists($ads_jar_path_and_filename)) {
            $failures[] = "ADS";
        }

        // Ensure no issues were detected
        if (count($failures) > 0) {
            $message = "Unable to Launch ADS Server: Missing "
                . implode(" and ", $failures);
            if (!is_null($this->ssh)) {
                $message .= " on remote host [{$this->host}]";
            }
            throw new Exception($message);
        }

        // Create the command for starting the ADS
        /**
         * Add `exec` to the ADS command to properly terminate process chain.
         *
         * NOTE: Due to some limitations in PHP, if you want to get the pid of a
         *       symfony Process, you may have to prefix your commands with exec.
         *       Please read Symfony Issue #5759 to understand why this is
         *       happening.
         *       https://github.com/symfony/symfony/issues/5759
         */
        if (is_null($this->ssh)) {
            $this->ads_command[] = "exec";
        }
        $this->ads_command[] = "java";
        $this->ads_command[] = "-jar";
        if (is_null($this->ssh)) {
            $this->ads_command[] = $ads_jar_path_and_filename;
        } else {
            // Copy the ADS jar file over to the remote host
            $filename = basename($ads_jar_path_and_filename);
            $remote_path_and_filename = $temporary_directory
                . $directory_separator . $filename;
            if (!ssh2_scp_send($this->ssh, $ads_jar_path_and_filename,
                    $remote_path_and_filename)) {
                throw new Exception("Unable to copy ADS file to "
                    . $this->host);
            }
            $this->ads_command[] = $remote_path_and_filename;
            $this->ads_command[] = "-a";
            $this->ads_command[] = $this->ssh_hostname;
        }
        $this->ads_command[] = "-k";
        $this->ads_command[] = "--confdir";
        $this->ads_command[] = $this->configuration_directory["server"];
    }

    public function __destruct() {
        // Terminate the process
        if (isset($this->process_id)) {
            if ($this->verbose) {
                echo "Terminating ADS Process: {$this->process_id}" . PHP_EOL;
            }
            $this->stop();
        }

        // Close the SSH session (if applicable)
        if (!is_null($this->ssh)) {
            fclose($this->process);
            unset($this->ssh);
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
        if (is_null($this->ssh)) {
            // Create the configuration directory for ADS
            @mkdir($this->configuration_directory["server"]);

            $process_builder = ProcessBuilder::create($this->ads_command)
                ->setEnv("KRB5_CONFIG", $this->configuration_file["server"]);
            $this->process = $process_builder->getProcess();

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
            $this->process_id = $this->process->getPid();

            // Wait for the ADS to be initialized
            foreach ($this->process as $type => $data) {
                if (stripos($data, "principal initialization complete") !== false) {
                    break;
                } else {
                    usleep(10000); // 100ms
                }
            }
        } else {
            // Create the configuration directory for the ADS on the remote host
            $make_directory_command = array();
            $make_directory_command[] = "mkdir";
            $make_directory_command[] = "-p";
            $make_directory_command[] = $this->configuration_directory["server"];
            $this->execute_command($make_directory_command, true);

            // Create the ADS command for SSH execution
            $args = array();
            foreach ($this->ads_command as $i => $arg) {
                $args[$i] = escapeshellarg($arg);
            }
            unset($arg);
            $command = implode(" ", $args);

            // Execute the command and return the output
            if ($this->verbose) {
                echo "ADS: {$command}" . PHP_EOL;
            }
            $this->process = ssh2_shell($this->ssh);
            if ($this->process) {
                fwrite($this->process, "export KRB5_CONFIG={$this->configuration_file["server"]};" . PHP_EOL);
                fwrite($this->process, "{$command} &" . PHP_EOL);
                stream_set_blocking($this->process, true);
                while ($buffer = fread($this->process, 4096)) {
                    flush();
                    if ($this->verbose) {
                        echo "ADS: {$buffer}";
                    }
                    if (stripos($buffer, "principal initialization complete") !== false) {
                        stream_set_blocking($this->process, false);
                        break;
                    } else {
                        usleep(10000); // 100ms
                    }
                }
                fwrite($this->process, "echo \$! > {$this->configuration_directory["server"]}ads.pid" . PHP_EOL);
                sleep(1);

                // Get the process ID for the running ADS instance
                $pid_command = array();
                $pid_command[] = "cat";
                $pid_command[] = $this->configuration_directory["server"] . "ads.pid";
                $this->process_id = trim($this->execute_command($pid_command, true)["output"]);

                // Copy the Kerberos files to the local machine
                @mkdir($this->configuration_directory["client"]);
                $kerberos_files = array(
                    $this->configuration_file["client"] => $this->configuration_file["server"],
                    $this->cassandra_keytab_file["client"] => $this->cassandra_keytab_file["server"],
                    $this->dse_keytab_file["client"] => $this->dse_keytab_file["server"],
                    $this->dseuser_keytab_file["client"] => $this->dseuser_keytab_file["server"],
                    $this->unknown_keytab_file["client"] => $this->unknown_keytab_file["server"]
                );
                foreach ($kerberos_files as $client_file => $server_file) {
                    if (!ssh2_scp_recv($this->ssh, $server_file, $client_file)) {
                        throw new Exception("Unable to copy server file "
                            . "{$server_file} from {$this->host}");
                    }
                }
            }
        }
    }

    /**
     * Stop the ADS server daemon and clean up any configuration directories
     */
    public function stop() {
        // Stop the process and remove the temporary directory
        if (is_null($this->ssh)) {
            if ($this->process->isRunning()) {
                $this->process->stop(0);
                IntegrationTest::remove_path($this->configuration_directory["server"]);
            }
        } else {
            // Create and execute the stop command
            $stop_command = array();
            $stop_command[] = "kill";
            $stop_command[] = "-9";
            $stop_command[] = $this->process_id;
            $this->execute_command($stop_command, true);

            // Create and execute the remove command
            $remove_command = array();
            $remove_command[] = "rm";
            $remove_command[] = "-rf";
            $remove_command[] = $this->configuration_directory["server"];
            $this->execute_command($remove_command, true);
            IntegrationTest::remove_path($this->configuration_directory["client"]);
        }
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

        // Output the current cache (debugging)
        if ($this->verbose) {
            $test = array();
            $test[] = "klist";
            $test[] = "-l";
            echo $this->execute_command($test)["output"] . PHP_EOL;
        }
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
     * @return bool True if the Kerberos client is available; false otherwise
     */
    private function is_kerberos_client_available() {
        $kerberos_client_available = array();
        $kerberos_client_available[] = "klist";
        $kerberos_client_available[] = "-V";
        return $this->execute_command($kerberos_client_available)["exit_code"] == 0;
    }

    /**
     * Determine if the PHP CLI is available on the remote host
     *
     * NOTE: SSH must be enabled
     *
     * @return bool True if the PHP CLI is available; false otherwise
     * @throws Exception If remote execution is requested and SSH is not enabled
     */
    private function is_php_cli_available() {
        $php_cli_available = array();
        $php_cli_available[] = "php";
        $php_cli_available[] = "--help";
        return $this->execute_command($php_cli_available, true)["exit_code"] == 0;
    }

    /**
     * Execute a command while applying the KRB5_CONFIG environment variable
     * pointing to the absolute path for the configuration file
     *
     * @param array $command Command to execute
     * @param bool $is_remote (Optional) True if remote execution should occur;
     *                                   false otherwise (default: false)
     * @return array Information about the command executed
     *     [
     *         "exit_code"    => (int) Exit code for the executed command
     *         "exit_message" => (string) Message if command had an error
     *         "output"       => (string) Output from the executed command
     *
     * @throws Exception If remote execution is requested and SSH is not enabled
     */
    private function execute_command(array $command, $is_remote = false) {
        // Determine if local or remote deployment is being used
        if (!$is_remote) {
            // Create the process for the command
            $process_builder = ProcessBuilder::create($command)
                ->setEnv("KRB5_CONFIG", $this->configuration_file["client"])
                ->setTimeout(60);
            /*
             * By default the Java Kerberos clients may be the first available
             * in the system path. This will update the `Path` to use the
             * common MIT Kerberos location(s) and prepend it to the `Path`
             * before execution of the command.
             *
             * NOTE: If Java Kerberos clients are used the ADS will not operate
             *       correctly and tests will fail.
             */
            if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' ||
                strtoupper(substr(PHP_OS, 0, 6)) === 'CYGWIN') {
                $program_files = array(
                    $_ENV["ProgramFiles"],
                    $_ENV["ProgramFiles(x86)"],
                    $_ENV["ProgramFilesW6432"],
                );
                foreach ($program_files as $path) {
                    $bin_path = $path
                        . DIRECTORY_SEPARATOR . "MIT"
                        . DIRECTORY_SEPARATOR . "Kerberos"
                        . DIRECTORY_SEPARATOR . "bin";
                    // Update the PATH (if exists)
                    if (file_exists($bin_path)) {
                        $process_builder
                            ->setEnv("Path", $bin_path . ";" . $_ENV["Path"]);
                        break;
                    }
                }
            }
            unset($path);
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

            // Get the process exit code and output
            $exit_code = $process->getExitCode();
            $exit_message = $process->getExitCodeText();
            $output = $process->getOutput();
        } else {
            // Determine if remote commands can be executed
            if (!is_null($this->ssh)) {
                // Get the command
                $args = array();
                foreach ($command as $i => $arg) {
                    $args[$i] = escapeshellarg($arg);
                }
                unset($arg);
                $command = implode(" ", $args);

                // Execute the command and return the output
                if ($this->verbose) {
                    echo "ADS: {$command}" . PHP_EOL;
                }
                $environment = "";
                if (isset($this->configuration_file["server"])) {
                    $environment = "KRB5_CONFIG={$this->configuration_file["server"]}";
                }
                $stream = ssh2_exec($this->ssh,
                    "{$environment} {$command} || echo 'COMMAND FAILED'"
                );
                if ($stream) {
                    stream_set_blocking($stream, true);
                    $output = "";
                    while ($buffer = fread($stream, 4096)) {
                        flush();
                        $output .= $buffer;
                    }
                    fclose($stream);
                    if ($this->verbose) {
                        if (!empty(trim($output))) {
                            echo "ADS: {$output}" . PHP_EOL;
                        }
                    }

                    // Get the process exit code and output
                    $exit_code = 0;
                    $exit_message = "";
                    if (preg_match("/^COMMAND FAILED$/", $output)) {
                        $exit_code = 255;
                        $exit_message = "{$command} command failed to execute";
                    }
                }
            } else {
                throw new Exception("Unable to execute remote command; enable SSH");
            }
        }

        // Return information about the process execution
        return array(
            "exit_code" => $exit_code,
            "exit_message" => $exit_message,
            "output" => $output
        );
    }

    /**
     * Get the directory separator on the local or remote host
     *
     * @return string Directory separator for local or remote host
     * @throws Exception If PHP CLI is missing on remote host (SSH only)
     */
    private function directory_separator() {
        if (is_null($this->ssh)) {
            return DIRECTORY_SEPARATOR;
        } else {
            if (!$this->is_php_cli_available()) {
                throw new Exception("Unable to Launch ADS Server: Missing "
                    . "PHP CLI on remote host [{$this->host}]");
            }
            // Create the command to get the directory separator
            $directory_separator_command = array();
            $directory_separator_command[] = "php";
            $directory_separator_command[] = "-r";
            $directory_separator_command[] = "echo DIRECTORY_SEPARATOR;";

            // Execute the command and return the directory separator
            return trim($this->execute_command($directory_separator_command, true)["output"]);

        }
    }

    /**
     * Get the temporary directory on the local or remote host
     *
     * @return string Temporary directory for local or remote host
     * @throws Exception If PHP CLI is missing on remote host (SSH only)
     */
    private function temporary_directory() {
        if (is_null($this->ssh)) {
            return sys_get_temp_dir();
        } else {
            if (!$this->is_php_cli_available()) {
                throw new Exception("Unable to Launch ADS Server: Missing "
                    . "PHP CLI on remote host [{$this->host}]");
            }
            // Create the command to get the temporary directory
            $temporary_directory_command = array();
            $temporary_directory_command[] = "php";
            $temporary_directory_command[] = "-r";
            $temporary_directory_command[] = "echo sys_get_temp_dir();";

            // Execute the command and return the temporary directory
            return trim($this->execute_command($temporary_directory_command, true)["output"]);

        }
    }

    /**
     * @param \Configuration|null $configuration Configuration settings to apply
     *                                           to ADS
     * @throws Exception If remote deployment is enabled and connection cannot
     *                    be established
     */
    private function process_configuration($configuration) {
        if (!is_null($configuration)) {
            $this->verbose = $configuration->verbose;

            // Determine if the deployment type is SSH
            if ($configuration->ssh_host) {
                if (function_exists("ssh2_connect")) {
                    $this->ssh = ssh2_connect($configuration->ssh_host,
                        $configuration->ssh_port);
                    if ($this->ssh) {
                        $this->host = $configuration->ssh_host;
                        if ($configuration->ssh_hostname) {
                            $this->ssh_hostname = $configuration->ssh_hostname;
                        }

                        // Determine if public/private key or password authentication
                        if ($configuration->ssh_public_key &&
                            $configuration->ssh_private_key) {
                            if (!ssh2_auth_pubkey_file($this->ssh,
                                $configuration->ssh_username,
                                $configuration->ssh_public_key,
                                $configuration->ssh_private_key,
                                $configuration->ssh_password)) {
                                throw new Exception("Unable to establish connection; "
                                    . "check public/private key credentials");
                            }
                        } else {
                            if (!ssh2_auth_password($this->ssh,
                                $configuration->ssh_username,
                                $configuration->ssh_password)) {
                                throw new Exception("Unable to establish connection; "
                                    . "check username/password credentials");
                            }
                        }
                    } else {
                        throw new Exception("Unable to establish connection to "
                            . $configuration->ssh_host);
                    }
                } else {
                    throw new Exception("SSH2 is not available");
                }
            }
        }
    }
}