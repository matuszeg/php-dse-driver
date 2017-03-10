<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;
use Behat\Behat\Hook\Scope\AfterFeatureScope;
use Behat\Behat\Hook\Scope\BeforeFeatureScope;
use Behat\Behat\Hook\Scope\AfterScenarioScope;
use Behat\Behat\Hook\Scope\BeforeScenarioScope;
use Behat\Testwork\Hook\Scope\BeforeSuiteScope;
use Behat\Testwork\Hook\Scope\HookScope;
use Symfony\Component\Process\PhpExecutableFinder;
use Symfony\Component\Process\PhpProcess;
use Symfony\Component\Process\ProcessBuilder;

/**
 * Defines the common traits that will be used in various feature contexts
 */
trait CommonTraits {
    /**
     * @var string Regular expression to retrieve keyspace name
     */
    private static $KEYSPACE_REGEX = "/create keyspace\\s+([a-zA-Z][a-zA-Z0-9_]+)\\s+/i";
    /**
     * @var string Prefix to use for CCM clusters
     */
    private static $CCM_CLUSTER_PREFIX = "php-driver_features";
    /**
     * @var CCM\Bridge CCM bridge to stand up Cassandra/DSE instances
     */
    private static $ccm;
    /**
     * @var Configuration Environment configuration/overrides for features
     */
    private static $configuration;
    /**
     * @var bool Flag to determine if the configuration settings should be
     *           displayed or not
     */
    private static $display_configuration = true;
    /**
     * @var string Absolute path to PHP executable
     */
    private static $php_executable = array();
    /**
     * @var string Example code to execute
     */
    private $example;
    /**
     * @var string Normalized output from executing the example code
     */
    private $example_output;
    /**
     * @var array INI settings to apply with example code
     */
    private $example_ini_settings = array();
    /**
     * @var PhpProcess PHP web server process
     */
    private $web_server_process;

    /**
     * Handle feature setup
     *
     * @param BeforeSuiteScope $scope The scope before the suite has
     *                                started
     * @BeforeSuite
     */
    public static function setup(BeforeSuiteScope $scope) {
        // Initialize the suite
        self::initialize($scope);
    }

    /**
     * Handle scenario setup for scenarios that will utilize a web server
     *
     * @param BeforeScenarioScope $scope The scope before the scenario has
     *                                  started
     * @BeforeScenario @web_server
     * @throws Exception If PHP executable could not be found
     */
    public function setup_scenario_web_server(BeforeScenarioScope $scope) {
        // Ensure the PHP process can be found
        $executableFinder = new PhpExecutableFinder();
        $php = $executableFinder->find();
        if ($php) {
            // Create the web server command
            $php_command = array();
            /**
             * Add `exec` to the PHP command to properly terminate process chain.
             *
             * NOTE: Due to some limitations in PHP, if you want to get the pid of a
             *       symfony Process, you may have to prefix your commands with exec.
             *       Please read Symfony Issue #5759 to understand why this is
             *       happening.
             *       https://github.com/symfony/symfony/issues/5759
             */
            if (strtoupper(substr(PHP_OS, 0, 3)) !== "WIN") {
                $php_command[] = "exec";
            }
            $php_command[] = $php;
            $php_command[] = "-S";
            $php_command[] = "127.0.0.1:10000";

            // Create the PHP web server process
            $process_builder = ProcessBuilder::create($php_command)
                ->setWorkingDirectory(sys_get_temp_dir());
            $this->web_server_process = $process_builder->getProcess();

            // Start the PHP web server
            $this->web_server_process->start(function ($type, $buffer) {
                if (self::$configuration->verbose) {
                    echo $buffer;
                }
            });
        } else {
            throw new \Exception("Unable to Start Web Server: PHP could not be found");
        }
    }

    /**
     * Handle scenario teardown for scenarios that utilize a web server
     *
     * @param AfterScenarioScope $scope The scope after the scenario has
     *                                 completed
     * @AfterScenario @web_server
     */
    public function teardown_scenario_web_server(AfterScenarioScope $scope) {
        $this->web_server_process->stop(0);
    }

    /**
     * Handle feature setup
     *
     * @param BeforeFeatureScope $scope The scope before the feature has
     *                                  started
     * @BeforeFeature
     */
    public static function setup_feature(BeforeFeatureScope $scope) {
        // Initialize the CCM instance
        self::initialize_ccm();
    }

    /**
     * Handle feature teardown
     *
     * @param AfterFeatureScope $scope The scope after the feature has
     *                                 completed
     * @AfterFeature
     */
    public static function teardown_feature(AfterFeatureScope $scope) {
        if (!self::$configuration->keep_clusters) {
            self::$ccm->remove_clusters();
        }
    }

    /**
     * Create a file with the given filename and contents
     *
     * NOTE: These files will be created in the system temp directory
     *
     * @param string $filename Filename to create
     * @param string $contents Contents of the file
     * @Given a file named :filename with:
     */
    public function a_file($filename, $contents) {
        // Create the pathing structure based on the filename
        $path = realpath(sys_get_temp_dir() . DIRECTORY_SEPARATOR
            . dirname($filename));
        if (!is_dir($path)) {
            mkdir($path, 0777, true);
        }

        // Create the file
        $path .= DIRECTORY_SEPARATOR . basename($filename);
        file_put_contents($path, $contents);
    }

    /**
     * Create a Cassandra/DSE cluster with a single node
     *
     * @param string $server_type Server type (e.g. Cassandra/DSE)
     * @Given /^a running (Cassandra|DSE) cluster$/
     */
    public function a_running_cluster($server_type) {
        $this->a_running_cluster_with_nodes($server_type, 1);
    }

    /**
     * Create a Cassandra/DSE cluster with the given number of nodes
     *
     * @param string $server_type Server type (e.g. Cassandra/DSE)
     * @param int $nodes
     * @param array $configuration Cassandra/DSE cluster configuration to apply
     * @Given /^a running (Cassandra|DSE) cluster with (\d+) nodes$/
     */
    public function a_running_cluster_with_nodes($server_type, $nodes,
                                                 $configuration = array()) {
        // Get the default CCM cluster configuration with the proper nodes
        $cluster = self::$ccm->default_cluster()
            ->add_data_center($nodes);

        // Determine if SSL needs to be enabled
        if (array_key_exists("ssl", $configuration)) {
            $cluster->with_ssl($configuration["ssl"]);

            // Determine if client authentication needs to be enabled
            if (array_key_exists("client_authentication", $configuration)) {
                $cluster->with_client_authentication($configuration["client_authentication"]);
            }
        }

        if (array_key_exists("workloads", $configuration)) {
            foreach ($configuration["workloads"] as $workload) {
                $cluster->add_workload($workload);
            }
        }

        if (self::$ccm->create_cluster($cluster)) {
            if (array_key_exists("cassandra", $configuration)) {
                self::$ccm->update_cluster_configuration($configuration["cassandra"]);
            }
            if (array_key_exists("dse", $configuration)) {
                self::$ccm->update_dse_cluster_configuration($configuration["dse"]);
            }

            // Start the cluster and assert that the cluster is ready
            PHPUnit_Framework_Assert::assertTrue(self::$ccm->start_cluster(
                array_key_exists("jvm", $configuration)
                    ? $configuration["jvm"]
                    : array()
            ));
        }
    }

    /**
     * Create a Cassandra/DSE cluster with SSL encryption enabled on a single
     * node
     *
     * @param string $server_type Server type (e.g. Cassandra/DSE)
     * @param string|null $client_authentication (Optional) If not null client
     *                                                      will be enabled
     *                                                      authentication
     * @Given /^a running (Cassandra|DSE) cluster with SSL encryption( and client authentication)?$/
     */
    public function a_running_cluster_with_ssl($server_type, $client_authentication = null) {
        // Create and update the configuration elements for SSL
        $configuration = array(
            "ssl" => true
        );

        // Determine if client authentication should be enabled
        if (!is_null($client_authentication)) {
            $configuration["client_authentication"] = true;
        }

        // Start the cluster with SSL enabled configuration
        $this->a_running_cluster_with_nodes($server_type, 1, $configuration);
    }

    /**
     * Create a web page with the given URI and contents
     *
     * @param string $uri URI to create
     * @param string $contents Contents of the web page
     * @Given a URI :uri with:
     */
    public function a_uri($uri, $contents) {
        // Create the web page
        $contents = $this->prepare_example($contents);
        $this->a_file($uri, $contents);
    }

    /**
     * Create a PHP example  to be executed from the example code
     *
     * @param PyStringNode $example Example code to use for isolated PHP
     *                              instance
     * @Given /^the following example:$/
     */
    public function given_example(PyStringNode $example) {
        $this->example = $this->prepare_example($example);
    }

    /**
     * Create/Execute the given schema definition; if the keyspace can be
     * determined from the schema a DROP CQL statement will be executed on the
     * keyspace
     *
     * @param PyStringNode $schema CQL/Schema code to execute via CCM
     * @Given /^the following schema:$/
     */
    public function given_schema($schema) {
        // Get the keyspace from the schema and drop the keyspace
        if (preg_match(self::$KEYSPACE_REGEX, (string) $schema, $matches)) {
            self::$ccm->execute_cql_on_node(1,
                "DROP KEYSPACE IF EXISTS {$matches[1]}");
        }

        // Create the schema using CQLSH
        self::$ccm->execute_cql_on_node(1, (string) $schema);
    }

    /**
     * Assign INI settings before the example code is executed
     *
     * @param string $setting_type Setting type to apply (e.g. INI/logger)
     * @param PyStringNode $settings INI settings to apply before executing
     *                               example code
     * @Given /^the following (INI|logger) settings:$/
     */
    public function given_settings($setting_type, PyStringNode $settings) {
        foreach ($settings->getStrings() as $setting) {
            $key_value = explode("=", $setting);
            if (count($key_value) == 2) {
                $this->example_ini_settings[$key_value[0]] = $key_value[1];
            }
        }
    }

    /**
     * Enable tracing on all nodes in the active cluster
     *
     * @Given /^tracing is enabled$/
     */
    public function given_tracing_enabled() {
        foreach (range(1, self::$ccm->cluster_status()["nodes"]) as $node) {
            self::$ccm->node_trace($node, true);
        }
    }

    /**
     * Execute the PHP example code
     *
     * @param array $configuration Optional configuration elements to apply to
     *                             the executing PHP process
     * @When /^it is executed$/
     */
    public function when_executed($configuration = array()) {
        // Create the process and apply the optional configurations
        $process_builder = ProcessBuilder::create(self::$php_executable)
            ->setWorkingDirectory(sys_get_temp_dir())
            ->setInput($this->example);
        if (array_key_exists("timeout", $configuration)) {
            $process_builder->setTimeout($configuration["timeout"]);
        }
        if (array_key_exists("environment", $configuration)) {
            foreach ($configuration["environment"] as $key => $value) {
                $process_builder->setEnv($key, $value);
            }
        }
        // Ensure the IP prefix is available for the features to use
        $process_builder->setEnv("IP_PREFIX", self::$ccm->ip_prefix());

        // Execute the PHP example code
        $process = $process_builder->getProcess();
        $process->run(function ($type, $buffer) {
            if (self::$configuration->verbose) {
                echo $buffer;
            }
        });
        if (self::$configuration->verbose) {
            echo "Process: [Exit Code: {$process->getExitCode()}] / "
                . "[Exit Message: {$process->getExitCodeText()}]" . PHP_EOL;
        }

        // Get the output and normalize the line endings
        $this->example_output = $process->getOutput();
        if (PHP_EOL !== "\n") {
            $this->example_output = str_replace(PHP_EOL, "\n", $this->example_output);
        }
        $this->example_output = trim(preg_replace("/ +$/m", "", $this->example_output));
    }

    /**
     * Execute the PHP example code with proper SSL credentials
     *
     * @When /^it is executed with proper SSL credentials$/
     */
    public function when_executed_with_proper_ssl_credentials() {
        // Assign the location of the SSL configuration file(s)
        $ssl_path = realpath(__DIR__ . DIRECTORY_SEPARATOR
            . ".." . DIRECTORY_SEPARATOR. ".." . DIRECTORY_SEPARATOR
            . "support" . DIRECTORY_SEPARATOR . "ssl") .  DIRECTORY_SEPARATOR;
        $server_certificate = $ssl_path . "cassandra.pem";
        $client_certificate = $ssl_path . "driver.pem";
        $private_key = $ssl_path . "driver.key";
        $passphrase = "cassandra";

        // Execute the example with the proper SSL credentials
        $this->when_executed(array("environment" => array(
            "SERVER_CERT" => $server_certificate,
            "CLIENT_CERT" => $client_certificate,
            "PRIVATE_KEY" => $private_key,
            "PASSPHRASE" => $passphrase
        )));
    }

    /**
     * Get the contents at the given URI
     *
     * @param string $uri URI to get contents
     * @param int $repeat Number of times to repeat navigation
     * @When /^I go to "(.*)"( \d+ times)?$/
     */
    public function when_i_go_to($uri, $repeat = 1) {
        $url = "http://127.0.0.1:10000" . str_replace("\"", "", $uri);
        foreach (range(1, intval($repeat)) as $i) {
            $this->example_output = file_get_contents($url);
        }
    }

    /**
     * Check to ensure that contents of a web URI contain all the elements in
     * the table
     *
     * @param TableNode $table Table to validate
     * @Then I should see:
     */
    public function then_i_should_see(TableNode $table) {
        // Determine which module we are looking for
        if (class_exists("Dse")) {
            $module = "dse";
        } else {
            $module = "cassandra";
        }

        // Parse the HTML for the loaded driver module
        $document = new DOMDocument();
        $document->loadHTML($this->example_output);
        $path = new DOMXpath($document);
        $nodes = $path->query("//h2/a[@name='module_${module}']/../following-sibling::*[position()=1][name()='table']");
        $html  = $nodes->item(0);

        // Validate each element in the table
        $table = $table->getRowsHash();
        foreach ($html->childNodes as $tr) {
            $name  = trim($tr->childNodes->item(0)->textContent);
            $value = trim($tr->childNodes->item(1)->textContent);
            if (isset($table[$name])) {
                PHPUnit_Framework_Assert::assertEquals($value, $table[$name],
                    "{$value} != {$table[$name]}");
                unset($table[$name]);
            }
        }

        // Ensure all the values have been found in the table
        if (!empty($table)) {
            PHPUnit_Framework_Assert::fail("Unable to Find Value: "
                . var_export($table, true));
        }
    }

    /**
     * Check to ensure a log file exists with the given name
     *
     * @param string $filename Name of log file expected to be found
     * @Then a log file :filename should exist
     */
    public function then_log_exists($filename) {
        PHPUnit_Framework_Assert::assertFileExists(
            sys_get_temp_dir() . DIRECTORY_SEPARATOR . $filename);
    }

    /**
     * Check to ensure a log file exists with the given name and that it
     * contains an expected value
     *
     * @param string $filename Name of log file expected to be found
     * @param string $expected Expected content(s) to be found in log filename
     * @Then the log file :filename should contain :log_level
     */
    public function then_log_contains($filename, $expected) {
        $this->then_log_exists($filename);
        PHPUnit_Framework_Assert::assertContains($expected,
            file_get_contents(sys_get_temp_dir() . DIRECTORY_SEPARATOR . $filename));
    }

    /**
     * Compare the output of the example
     *
     * @param PyStringNode $expected Expected output
     * @Then its output should contain:
     */
    public function then_output_contains(PyStringNode $expected) {
        if (empty((string)$expected)) {
            PHPUnit_Framework_Assert::assertEmpty($this->example_output, $this->example_output);
        } else {
            PHPUnit_Framework_Assert::assertContains((string) $expected,
                $this->example_output);
        }
    }

    /**
     * Compare the output of the example disregarding the order
     *
     * @param PyStringNode $expected Expected output
     * @Then /^its output should contain disregarding order:$/
     */
    public function then_output_contains_disregarding_order(PyStringNode $expected) {
        // Sort the expected and actual output values
        $expected = $expected->getStrings();
        $actual = explode("\n", $this->example_output);
        sort($expected);
        sort($actual);
        $expected = implode(PHP_EOL, $expected);
        $actual = implode(PHP_EOL, $actual);

        PHPUnit_Framework_Assert::assertContains($expected, $actual);
    }

    /**
     * Compare the output of the example using regular expression pattern
     *
     * @param PyStringNode $expected Expected output
     * @Then /^its output should contain pattern:$/
     */
    public function then_output_contains_pattern(PyStringNode $expected) {
        PHPUnit_Framework_Assert::assertRegExp("/" . (string) $expected . "/",
            $this->example_output);
    }

    /**
     * Apply/Prepare example for execution
     *
     * This method will handle ini setting updates as well as prepare any
     * clusters for remote CCM instances
     *
     * @param PyStringNode $example Content/Example to prepare
     * @return string Prepared content/example
     */
    private function prepare_example(PyStringNode $example) {
        $autoload_file = realpath(__DIR__ . "/../../vendor/autoload.php");
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' ||
            strtoupper(substr(PHP_OS, 0, 6)) === 'CYGWIN') {
            $autoload_file = str_replace("\\", "\\\\", $autoload_file);
        }
        $preparation = "include \"" . $autoload_file . "\";" . PHP_EOL
            . "ini_set(\"date.timezone\", \"America/New_York\");";
        foreach ($this->example_ini_settings as $setting => $value) {
            $preparation .= PHP_EOL . "ini_set(\"{$setting}\", \"{$value}\");" . PHP_EOL;
        }
        if (self::$configuration->ssh_host) {
            $contact_point = self::$configuration->ssh_host;
            $example = str_replace("Dse::cluster()",
                "Dse::cluster()->withContactPoints(\"{$contact_point}\")",
                (string) $example);
            $example = str_replace("Cassandra::cluster()",
                "Cassandra::cluster()->withContactPoints(\"{$contact_point}\")",
                (string) $example);
        }
        return "<?php" . PHP_EOL
            . $preparation . PHP_EOL
            . $example;
    }

    /**
     * Multidimensional search for value in array
     *
     * @param string $key Key to search for
     * @param array $array Array to search
     * @return mixed Value found for key
     */
    private static function get_value($key, array $array) {
        // Check if the key is in the base array
        if (array_key_exists($key, $array)) {
            return $array["{$key}"];
        } else {
            // Check multidimensional array
            foreach ($array as $portion) {
                if (is_array($portion)) {
                    $value = self::get_value($key, $portion);
                    if ($value) {
                        return $value;
                    }
                }
            }
        }

        // Unable to find key in array
        return false;
    }

    /**
     * Get the default session. This method should be used for setting up the
     * features using the driver.
     *
     * @return Dse\Session Default session instance
     */
    private static function default_session() {
        // Determine if remote host (SSH) is being used or local instances
        $cluster = Dse::cluster();
        if (self::$configuration->ssh_host) {
            $cluster->withContactPoints(self::$configuration->ssh_host);
        }
        return $cluster->build()->connect();
    }

    /**
     * Initialization for the context feature
     *
     * @param HookScope $scope Scope to gather initialization information from
     * @throws \Exception If Cassandra/DSE version has not been assigned
     */
    private static function initialize(HookScope $scope) {
        // Get the environmental configurations
        self::$configuration = new Configuration();
        self::$configuration->prefix = self::$CCM_CLUSTER_PREFIX;
        // Override the verbose setting if certain PHPUnit arguments are used
        if (in_array("-v", $_SERVER['argv']) ||
            in_array("--verbose", $_SERVER['argv'])) {
            self::$configuration->verbose = true;
        }

        // Get the version from the feature and update the configuration
        $settings = $scope->getEnvironment()
            ->getSuite()
            ->getSettings();
        if (self::get_value("cassandra_version", $settings)) {
            self::$configuration->dse = false;
            self::$configuration->version =
                new Cassandra\Version(self::get_value("cassandra_version", $settings));
        } else if (self::get_value("dse_version", $settings)) {
            self::$configuration->dse = true;
            self::$configuration->version =
                new Dse\Version(self::get_value("dse_version", $settings));
        } else {
            throw new \Exception("Cassandra/DSE version has not been assigned");
        }

        // Print the current settings (will be performed only once)
        self::print_settings();

        // Get the PHP process (locate only once)
        if (empty(self::$php_executable)) {
            /**
             * Add `exec` to the ADS command to properly terminate process
             * chain.
             *
             * NOTE: Due to some limitations in PHP, if you want to get the pid
             *       of a symfony Process, you may have to prefix your commands
             *       with exec.
             *       Please read Symfony Issue #5759 to understand why this is
             *       happening.
             *       https://github.com/symfony/symfony/issues/5759
             */
            if ("\\" !== DIRECTORY_SEPARATOR) {
                self::$php_executable[] = "exec";
            }

            // Locate the PHP executable
            $finder = new PhpExecutableFinder();
            if (false === self::$php_executable[] = $finder->find()) {
                throw new \Exception("Unable to locate PHP executable");
            }
        }
    }

    /**
     * Initialize the CCM instance
     */
    private static function initialize_ccm() {
        // Initialize CCM instance
        self::$ccm = new CCM\Bridge(self::$configuration);

        // Clear any clusters that may have been left over from previous tests
        if (!self::$configuration->keep_clusters) {
            self::$ccm->remove_clusters();
        }
    }

    /**
     * Display the current configuration settings
     *
     * NOTE: This will only be displayed once and only if verbosity is enabled
     */
    private static function print_settings() {
        if (self::$display_configuration && self::$configuration->verbose) {
            // Display information about the current setup for the tests being run
            echo "Starting DataStax PHP Driver Feature Test" . PHP_EOL
                . "  PHP v" . phpversion() . PHP_EOL;
            self::$configuration->print_settings();

            // Indicate the configuration settings have been displayed
            self::$display_configuration = false;
        }
    }
}
