<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Behat\Gherkin\Node\PyStringNode;
use Behat\Behat\Hook\Scope\AfterFeatureScope;
use Behat\Behat\Hook\Scope\BeforeFeatureScope;
use Behat\Testwork\Hook\Scope\BeforeSuiteScope;
use Behat\Testwork\Hook\Scope\HookScope;
use Symfony\Component\Process\PhpProcess;

/**
 * Defines the common traits that will be used in various feature contexts
 */
trait CommonTraits {
    /**
     * @var CCM\Bridge CCM bridge to stand up Cassandra/DSE instances
     */
    private static $ccm;
    /**
     * @var Configuration Environment configuration/overrides for features
     */
    private static $configuration;
    /**
     * @var string Example code to execute
     */
    private $example;
    /**
     * @var string Normalized output from executing the example code
     */
    private $example_output;

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
     * Handle feature setup
     *
     * @param BeforeFeatureScope $scope The scope before the feature has
     *                                  started
     * @BeforeFeature
     */
    public static function setup_feature(BeforeFeatureScope $scope) {
        // Initialize the CCM instance
        self::initialize_ccm("feature");
    }

    /**
     * Handle feature teardown
     *
     * @param AfterFeatureScope $scope The scope after the feature has
     *                                 completed
     * @AfterFeature
     */
    public static function teardown_feature(AfterFeatureScope $scope) {
        self::$ccm->remove_clusters();
    }

    /**
     * Create a Cassandra/DSE cluster with a single node
     *
     * @Given a running (Cassandra|DSE) cluster
     */
    public function a_running_cluster() {
        $this->a_running_cluster_with_nodes(1);
    }

    /**
     * Create a Cassandra/DSE cluster with the given number of nodes
     *
     * @param int $nodes
     * @param array $configuration Cassandra/DSE cluster configuration to apply
     * @Given a running (Cassandra|DSE) cluster with :nodes nodes
     */
    public function a_running_cluster_with_nodes($nodes, $configuration = array()) {
        if (self::$ccm->create_cluster(self::$ccm->default_cluster()
            ->add_data_center($nodes))) {
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
     * Create a PHP file with the example code
     *
     * @param PyStringNode $example Example code to use for isolated PHP
     *                              instance
     * @Given /^the following example:$/
     */
    public function given_example(PyStringNode $example) {
        $include = "include \"" . realpath(__DIR__ . "/../../vendor/autoload.php") . "\";";
        $this->example = "<?php " . PHP_EOL
            . $include . PHP_EOL
            . $example;
        echo $this->example . PHP_EOL;
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
        $process = new PhpProcess($this->example);
        if (array_key_exists("timeout", $configuration)) {
            $process->setTimeout($configuration["timeout"]);
        }
        if (array_key_exists("environment", $configuration)) {
            $process->setEnv($configuration["environment"]);
        }

        // Execute the PHP example code
        $process->run(function ($type, $buffer) {
            if (self::$configuration->verbose) {
                echo $buffer;
            }

            // Normalize the line endings in the output
            $output = $buffer;
            if (PHP_EOL !== "\n") {
                $output = str_replace(PHP_EOL, "\n", $buffer);
            }
            $output = trim(preg_replace('/ +$/m', '', $output));

            // Append the output to the example output
            $this->example_output .= $output;
        });
    }

    /**
     * Compare the output of the example
     *
     * @param PyStringNode $expected Expected output
     * @Then its output should contain:
     */
    public function then_output_contains(PyStringNode $expected) {
        PHPUnit_Framework_Assert::assertContains((string) $expected,
            $this->example_output);
    }

    /**
     * Compare the output of the example using regular expression pattern
     *
     * @param PyStringNode $expected Expected output
     * @Then its output should contain pattern:
     */
    public function then_output_contains_pattern(PyStringNode $expected) {
        PHPUnit_Framework_Assert::assertRegExp("/" . (string) $expected . "/",
            $this->example_output);
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
     * Initialization for the context feature
     *
     * @param HookScope $scope Scope to gather initialization information from
     * @throws \Exception If Cassandra/DSE version has not been assigned
     */
    private static function initialize(HookScope $scope) {
        // Get the environmental configurations
        self::$configuration = new Configuration();

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
    }

    /**
     * Initialize the CCM instance
     *
     * @param string|null $prefix Prefix to assign for CCM clusters
     */
    private static function initialize_ccm($prefix = null) {
        // Initialize CCM instance
        if (!is_null($prefix) &&
            substr(self::$configuration->prefix, -strlen($prefix)) !== $prefix) {
            self::$configuration->prefix .= "_{$prefix}";
        }
        self::$ccm = new CCM\Bridge(self::$configuration);

        // Clear any clusters that may have been left over from previous tests
        self::$ccm->remove_clusters();
    }
}