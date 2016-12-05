<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * This class will act as a fixture for the integration test suite. This
 * fixture will ensure startup and shutdown procedures when running the
 * integration tests.
 */
class IntegrationTestFixture {
    /**
     * @var IntegrationTestFixture Singleton instance for the fixture
     */
    private static $instance;
    /**
     * @var \CCM\Bridge CCM instance for working with Cassandra/DSE clusters
     */
    private $ccm;
    /**
     * @var Configuration Configuration settings for the test harness
     */
    private $configuration;

    public function __destruct() {
        $this->ccm->remove_clusters();
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            return $this->$property;
        }
    }

    /**
     * Create the integration test fixture for performing startup and shutdown
     * procedures required by the integration test suite.
     */
    public static function get_instance() {
        // Ensure only one instance (singleton)
        if (!isset(self::$instance)) {
            self::$instance  = new IntegrationTestFixture();
        }
        return self::$instance;
    }

    private function __construct() {
        // Create the configuration instance for the entire run of the integration tests
        $this->configuration = new Configuration();
        // Override the verbose setting if certain PHPUnit arguments are used
        if (in_array("--debug", $_SERVER['argv']) ||
            in_array("--verbose", $_SERVER['argv'])) {
            $this->configuration->verbose = true;
        }

        // Display information about the current setup for the tests being run
        echo "Starting DataStax PHP Driver Integration Test" . PHP_EOL
            . "  PHP v" . phpversion() . PHP_EOL
            . "  Cassandra driver v" . \Cassandra::VERSION . PHP_EOL
            . "  DSE driver v" . \Dse::VERSION . PHP_EOL
            . "  C/C++ DSE drvier v" . \Dse::CPP_DRIVER_DSE_VERSION . PHP_EOL;
        $this->configuration->print_settings();
        echo PHP_EOL;

        // Create the CCM instance for the entire run of the integration tests
        $this->ccm = new CCM\Bridge($this->configuration);

        // Clear any clusters that may have been left over from previous tests
        $this->ccm->remove_clusters();
    }
}

/**
 * Base class to provide common integration test functionality.
 */
abstract class IntegrationTest extends \PHPUnit_Framework_TestCase {
    /**
     * Create keyspace format
     */
    const CREATE_KEYSPACE_FORMAT = "CREATE KEYSPACE %s WITH replication = %s;";
    /**
     * Drop keyspace format
     */
    const DROP_KEYSPACE_FORMAT = "DROP KEYSPACE '%s';";
    /**
     * Maximum length for the keyspace (server limit)
     */
    const KEYSPACE_MAXIMUM_LENGTH = 48;
    /**
     * Replication strategy format
     */
    const REPLICATION_STRATEGY_FORMAT = "{ 'class': %s }";

    /**
     * @var \CCM\Bridge CCM interface instance
     */
    protected static $ccm;
    /**
     * @var \CCM\Cluster CCM cluster configuration to create
     */
    protected static $cluster_configuration;
    /**
     * @var \Configuration Test harness configuration settings
     */
    protected static $configuration;

    /**
     * @var \Cassandra\Cluster|\Dse\Cluster Cassandra/DSE cluster
     *
     * @see \Cassandra\Cluster
     * @see \Dse\Cluster
     */
    protected $cluster;
    /**
     * @var string Create keyspace query to use for the test(s)
     */
    protected $create_keyspace_query;
    /**
     * @var bool True if session should be established; false otherwise
     */
    protected $create_session = true;
    /**
     * @var string Keyspace to use for the test(s)
     */
    protected $keyspace;
    /**
     * @var string Replication factor override; default is calculated based on
     *             number of data center nodes; single data center is
     *             (nodes / 2) rounded up
     */
    protected $replication_factor = 0;
    /**
     * @var string Replication strategy based on the data center arrangement in
     *             the \CCM\Cluster configuration
     */
    protected $replication_strategy;
    /**
     * @var \Cassandra\Version|\Dse\Version Version of the server connected to
     *
     * @see \Cassandra\Version
     * @see \Dse\Version
     */
    protected $server_version;
    /**
     * @var \Cassandra\Session|\Dse\Session Cassandra/DSE session
     *
     * @see \Cassandra\Session
     * @see \Dse\Session
     */
    protected $session;
    /**
     * @var bool True if CCM cluster should be started; false otherwise
     */
    protected $start_cluster = true;
    /**
     * @var string Table name to use for the test(s)
     */
    protected $table;

    /**
     * @inheritdoc
     */
    public static function setUpBeforeClass() {
        // Initializes the fixture and get the test lifetime instances
        $fixture = IntegrationTestFixture::get_instance();

        // Get the CCM global instance and create the initial cluster
        self::$ccm = $fixture->ccm;
        self::$cluster_configuration = self::$ccm->default_cluster();

        // Assign the configuration from the global instance
        self::$configuration = $fixture->configuration;
    }

    /**
     * Add custom @requires for specific Cassandra/DSE version requirements
     *
     * NOTE: If server configuration is using DSE and requirement is for
     *       Cassandra, the internal Cassandra version of DSE will be utilized
     *       for the comparison
     *
     * Example (>=): @requires Cassandra 3.0.0
     * Example (>=): @requires DSE >= 5.0.0
     * Example (==): @requires DSE == 4.8.11
     *
     * @inheritdoc
     */
    protected function checkRequirements() {
        parent::checkRequirements();

        $annotations = $this->getAnnotations();
        foreach (array("class", "method") as $depth) {
            // Short circuit if no annotation are available
            if (empty($annotations[$depth]["requires"])) {
                continue;
            }

            // Go through each of the annotations and parse the Cassandra/DSE version
            foreach ($annotations[$depth]["requires"] as $annotation) {
                // Ensure the server type is Cassandra or DSE and gather the requirements
                $requires = explode(" ", $annotation);
                $type = $requires[0];
                if ($type == "Cassandra" || $type == "DSE") {
                    // Handle default comparison operator
                    if (count($requires) == 2) {
                        $operator = ">=";
                        $version = $requires[1];
                    } else if (count($requires) == 3) {
                        $operator = $requires[1];
                        $version = $requires[2];
                    } else {
                        continue;
                    }

                    // Get the version information from the configuration
                    $server_type = "Cassandra";
                    $server_version = self::$configuration->version;
                    if (self::$configuration->dse) {
                        $server_type = "DSE";
                        if ($type == "Cassandra") {
                            $server_version = self::$configuration->version->cassandra_version;
                        }
                    }

                    // Determine if the test should be skipped
                    if (!version_compare($server_version, $version, $operator)) {
                        $this->markTestSkipped("{$type} {$operator} v{$version} "
                            . "is Required; {$server_type} v{$server_version} is in Use: "
                            . "Skipping {$this->toString()}");
                    }
                }
            }
        }
    }

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Determine if the data center nodes should be initialized (default 1 node)
        if (count(self::$cluster_configuration->data_centers) == 0) {
            self::$cluster_configuration->add_data_center();
        }

        // Generate the keyspace name; apply unique id if keyspace is to long
        $this->keyspace = $this->get_short_name() . "_" . $this->getName(false);
        if (strlen($this->keyspace) > self::KEYSPACE_MAXIMUM_LENGTH) {
            // Update the keyspace name with a unique ID
            $unique_id = uniqid();
            $this->keyspace = substr($this->keyspace,
                    0, self::KEYSPACE_MAXIMUM_LENGTH - strlen($unique_id)) .
                $unique_id;
        }
        $this->keyspace = str_replace("test", "", strtolower($this->keyspace));

        // Generate the table name
        $this->table = str_replace("test", "", strtolower($this->getName(false)));

        // Determine replication strategy
        $replication_strategy = "'SimpleStrategy', 'replication_factor': ";
        if (count(self::$cluster_configuration->data_centers) > 1) {
            $replication_strategy = "'NetworkTopologyStrategy',";
            foreach (self::$cluster_configuration->data_centers
                     as $data_center => $nodes) {
                $replication_strategy .= " 'dc{$data_center}': {$nodes},";
            }
        } else {
            if ($this->replication_factor == 0) {
                $nodes = self::$cluster_configuration->data_centers[0];
                $this->replication_factor = ($nodes % 2 == 0)
                    ? ($nodes / 2)
                    : (($nodes + 1) / 2);
            }
            $replication_strategy .= $this->replication_factor;
        }
        $this->replication_strategy = sprintf(self::REPLICATION_STRATEGY_FORMAT,
            $replication_strategy);

        // Generate the keyspace query
        $this->create_keyspace_query = sprintf(self::CREATE_KEYSPACE_FORMAT,
            $this->keyspace, $this->replication_strategy);

        // Create and start the CCM cluster
        $this->create_server_cluster();
        if ($this->start_cluster) {
            $this->start_server_cluster();
            if ($this->create_session) {
                // Establish the session connection
                $this->cluster = $this->default_cluster();
                $connection = $this->connect_cluster();
                $this->session = $connection["session"];
                $this->server_version = $connection["version"];
            }
        }
    }

    /**
     * @inheritdoc
     */
    protected function tearDown() {
        // Drop keyspace for integration test (may or may have not been created)
        if (!is_null($this->session)) {
            try {
                $query = sprintf(self::DROP_KEYSPACE_FORMAT, $this->keyspace);
                $statement = new \Cassandra\SimpleStatement($query);
                $this->session->execute($statement);
            } catch (\Exception $e) {
                ; // no-op
            }
        }

        // Reset the default cluster (in the event it was changed)
        self::$cluster_configuration = self::$ccm->default_cluster();
    }

    /**
     * Start the active Cassandra/DSE cluster (Asserts that the cluster is
     * up and available)
     *
     * @param array $jvm_arguments (Optional) Cassandra/DSE server JVM
     *                                        arguments (default: none)
     *     The optional JVM arguments can be a simple array of JVM arguments
     *     or it can be an array that contains the JVM arguments accessed by a
     *     key into the array:
     *     1.
     *         [
     *             "-Dproperty=value",
     *             "-Dproperty=value",
     *             .
     *             .
     *             "-Dproperty=value"
     *         ]
     *     2.
     *         [
     *             "jvm" => (array[string]) JVM arguments to apply (see (1) for
     *                                      format
     *         ]
     */
    public function start_server_cluster($jvm_arguments = array()) {
        // Start the cluster and assert that the cluster is ready
        PHPUnit_Framework_Assert::assertTrue(self::$ccm->start_cluster(
            array_key_exists("jvm", $jvm_arguments)
                ? $jvm_arguments["jvm"]
                : array()
        ), (self::$configuration->dse ? "DSE" : "Cassandra")
            . " cluster was not started properly");
    }

    /**
     * Connect to the Cassandra/DSE cluster and get the version information from
     * the server (may be different than the configuration settings version
     * used)
     *
     * @param \Cassandra\Session|\Dse\Session|null $cluster (Optional) Cluster instance
     *                                                                 (default: base
     *                                                                 cluster instance)
     * @param string|null $query (Optional) Query to execute upon connection
     *                                      (default: generated create keyspace
     *                                                query)
     * @return array Cassandra/DSE session and server version from established
     *               connection
     *     [
     *         "rows"  => (\Cassandra\Rows) Query result (rows)
     *         "session" => (\Cassandra\Session|\Dse\Session) Session instance
     *         "version" => (\Cassandra\Version|\Dse\Version) Server version
     *     ]
     *
     * @see \Cassandra\Session
     * @see \Dse\Session
     * @see \Cassandra\Version
     * @see \Dse\Version
     */
    protected function connect_cluster($cluster = null, $query = null) {
        // Establish the connection to the server
        if (is_null($cluster)) {
            $cluster = $this->cluster;
        }
        $session = $cluster->connect();

        // Get the server version
        if (self::$configuration->dse) {
            $version = Dse\Version::from_server($session);
        } else {
            $version = Cassandra\Version::from_server($session);
        }

        // Create the keyspace
        if (is_null($query)) {
            $query = $this->create_keyspace_query;
        }
        if (self::$configuration->verbose) {
            echo "Executing Query: {$query}" . PHP_EOL;
        }
        $statement = new \Cassandra\SimpleStatement($query);
        $rows = $session->execute($statement);

        // Return the result, session and version
        return array(
            "rows" => $rows,
            "session" => $session,
            "version" => $version
        );
    }

    /**
     * Create the Cassandra/DSE cluster
     *
     * @param \CCM\Cluster|null $cluster_configuration (Optional) CCM cluster
     *                                                            configuration
     * @param array $server_configuration (Optional) Cassandra/DSE server
     *                                               configuration
     *     [
     *         "cassandra" => Cassandra update configurations
     *         "dse" => DSE update configurations
     *     ]
     * @return bool True if cluster was created or switched; false otherwise
     * @throws \Exception If no data centers exist in the cluster
     */
    protected function create_server_cluster(\CCM\Cluster $cluster_configuration = null,
                                             $server_configuration = array()) {
        // Determine if the base cluster configuration should be used
        if (is_null($cluster_configuration)) {
            $cluster_configuration = self::$cluster_configuration;
        }

        // Create the cluster
        $created_switched = self::$ccm->create_cluster($cluster_configuration);
        if ($created_switched) {
            if (array_key_exists("cassandra", $server_configuration)) {
                self::$ccm->update_cluster_configuration($server_configuration["cassandra"]);
            }
            if (array_key_exists("dse", $server_configuration)) {
                self::$ccm->update_dse_cluster_configuration($server_configuration["dse"]);
            }
        }
        return $created_switched;
    }

    /**
     * Get the default cluster
     *
     * @return \Cassandra\Cluster|\Dse\Cluster Cassandra or DSE cluster
     *
     * @see \Cassandra\Cluster
     * @see \Dse\Cluster
     */
    protected function default_cluster() {
        $contact_points = self::$ccm->contact_points();

        // Determine if the DSE cluster should be created
        if (self::$configuration->dse) {
            return \Dse::cluster()
                ->withContactPoints($contact_points)
                ->withSchemaMetadata(false)
                ->build();
        }

        // Create the default Cassandra cluster
        return \Cassandra::cluster()
            ->withContactPoints($contact_points)
            ->withSchemaMetadata(false)
            ->build();
    }

    /**
     * Get the short name of the class without the namespacing.
     *
     * @return string Short name for the class name
     */
    private function get_short_name() {
        $class = new \ReflectionClass($this);
        return $class->name;
    }
}