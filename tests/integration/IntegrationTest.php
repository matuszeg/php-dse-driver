<?php

// Create an alias for DSE extension to share core test framework
use \Dse as Cassandra;
use \Cassandra\Version as CassandraVersion;
use const \Dse\VERSION;
use const \Dse\CPP_DRIVER_DSE_VERSION as CPP_DRIVER_VERSION;

// Create global constants for the logging options
/**
 * Filename option for the logger
 */
const LOG_FILENAME_OPTION = "dse.log";
/**
 * Log level option for the logger
 */
const LOG_LEVEL_OPTION = "dse.log_level";

/**
 * Copyright (c) 2017 DataStax, Inc.
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
     * @var CCM\Bridge CCM instance for working with Cassandra/DSE clusters
     */
    private $ccm;
    /**
     * @var Configuration Configuration settings for the test harness
     */
    private $configuration;

    public function __destruct() {
        if (!is_null($this->ccm)) {
            if (!$this->configuration->keep_clusters) {
                $this->ccm->remove_clusters();
            }
        }
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
        try {
            $this->configuration = new Configuration();
        } catch (\Exception $e) {
            exit($e);
        }
        // Override the verbose setting if certain PHPUnit arguments are used
        if (in_array("--debug", $_SERVER['argv']) ||
            in_array("--verbose", $_SERVER['argv'])) {
            $this->configuration->verbose = true;
        }

        // Display information about the current setup for the tests being run
        echo PHP_EOL . "Starting DataStax PHP Driver Integration Test" . PHP_EOL
            . "  PHP v" . phpversion() . PHP_EOL
            . "  PHP ";
        if (class_exists("Dse")) {
            echo "DSE";
        } else {
            echo "Cassandra";
        }
        echo " Extension v" . Cassandra::VERSION . PHP_EOL
            . "  C/C++ ";
        if (class_exists("Dse")) {
            echo "DSE v" . Dse::CPP_DRIVER_DSE_VERSION . "/";
        }
        echo "v" . Cassandra::CPP_DRIVER_VERSION . PHP_EOL;
        $this->configuration->print_settings();
        echo PHP_EOL;

        // Create the CCM instance for the entire run of the integration tests
        try {
            $this->ccm = new CCM\Bridge($this->configuration);
        } catch (\Exception $e) {
            exit($e->getCode());
        }

        // Clear any clusters that may have been left over from previous tests
        if (!$this->configuration->keep_clusters) {
            $this->ccm->remove_clusters();
        }

        // Remove the log file(s)
        IntegrationTest::remove_path("logs");
    }
}

/**
 * Base class to provide common integration test functionality.
 */
abstract class IntegrationTest extends \PHPUnit_Framework_TestCase {
    /**
     * Minimum value for integer
     */
    const PHP_INT_MIN = -PHP_INT_MAX - 1;
    /**
     * Create keyspace format
     */
    const CREATE_KEYSPACE_FORMAT = "CREATE KEYSPACE %s WITH replication = %s;";
    /**
     * Create UDT format
     */
    const CREATE_TYPE_FORMAT = "CREATE TYPE IF NOT EXISTS %s.%s (%s)";
    /**
     * Drop keyspace format
     */
    const DROP_KEYSPACE_FORMAT = "DROP KEYSPACE %s;";
    /**
     * Maximum length for the keyspace (server limit)
     *
     * NOTE: The actual limit is 48; however due to graph names creating
     *       keyspaces with additional suffix information, the keyspace
     *       length has been reduced to 40
     */
    const KEYSPACE_MAXIMUM_LENGTH = 48;
    /**
     * Maximum length for the table (server limit)
     *
     * NOTE: The actual limit is 48; however due to graph names creating
     *       keyspaces with additional suffix information, the table length
     *       has been reduced to 40 since the table name is used for most graph
     *       names in tests.
     */
    const TABLE_MAXIMUM_LENGTH = 40;
    /**
     * Replication strategy format
     */
    const REPLICATION_STRATEGY_FORMAT = "{ 'class': %s }";
    /**
     * Conversion value for seconds to milliseconds.
     */
    const SECONDS_TO_MILLISECONDS = 1000;
    /**
     * Conversion value for seconds to microseconds.
     */
    const SECONDS_TO_MICROSECONDS = 1000000;
    /**
     * Select format for the data types (select cql query)
     */
    const SELECT_FORMAT = "SELECT id, value FROM %s.%s";

    /**
     * @var CCM\Bridge CCM interface instance
     */
    protected static $ccm;
    /**
     * @var CCM\Cluster CCM cluster configuration to create
     */
    protected static $cluster_configuration;
    /**
     * @var \Configuration Test harness configuration settings
     */
    protected static $configuration;

    /**
     * @var Cassandra\Cluster Cluster instance
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
     * @var string Directory/Path where the log file will be saved
     */
    protected $log_directory;
    /**
     * @var string Filename being used for the current tests logging (driver)
     */
    protected $log_filename;
    /**
     * @var resource File handle for log file being monitored
     */
    protected $log_handle = null;
    /**
     * @var int Replication factor override; default is calculated based on
     *          number of data center nodes; single data center is (nodes / 2)
     *          rounded up
     */
    protected $replication_factor = 0;
    /**
     * @var string Replication strategy based on the data center arrangement in
     *             the CCM\Cluster configuration
     */
    protected $replication_strategy;
    /**
     * @var Cassandra\Version|Dse\Version Version of the server connected to
     *
     * @see Cassandra\Version
     * @see Dse\Version
     */
    protected $server_version;
    /**
     * @var Cassandra\Session Session instance
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
     * @var bool True if test case is using a data provider; false otherwise
     *
     * NOTE: This will effect the table name and log file name
     */
    protected $using_data_provider = false;

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
     * Remove all files and directories from a path
     *
     * @param $path Path to remove all files and directories from
     */
    public static function remove_path($path) {
        // Iterate over each element in the path and remove all elements
        if (file_exists($path)) {
            // Read the contents of the path (removing unnecessary paths)
            $files = array_diff(scandir($path), array('..', '.'));
            foreach ($files as $file) {
                // Determine if the file should be deleted or the directory
                $full_path = $path . DIRECTORY_SEPARATOR . $file;
                if (is_file($full_path)) {
                    @unlink($full_path);
                } else if (is_dir($full_path)) {
                    self::remove_path($full_path);
                }
            }

            // Remove the path
            @rmdir($path);
        }
    }

    /**
     * Add custom @requires for specific Cassandra/DSE version requirements and
     * C/C++ driver version requirements along with additional annotations for
     * skipping tests (@skip), marking tests incomplete (@incomplete), and also
     * marking tests as unstable (@unstable)
     *
     * NOTE: Unstable tests can still be executed if enabled via environmental
     *       configuration option
     *
     * NOTE: If server configuration is using DSE and requirement is for
     *       Cassandra, the internal Cassandra version of DSE will be utilized
     *       for the comparison
     *
     * Example (>=): @requires Cassandra 3.0.0
     * Example (>=): @requires DSE >= 5.0.0
     * Example (==): @requires DSE == 4.8.0
     * Example (>=): @requires cpp-driver >= 2.5.0
     * Example (>=): @requires cpp-driver-dse >= 1.2.0
     *
     * @inheritdoc
     */
    protected function checkRequirements() {
        parent::checkRequirements();

        // Get all the annotations for the test being performed
        $annotations = $this->getAnnotations();
        foreach (array("class", "method") as $depth) {
            // Short circuit if no annotation are available
            if (empty($annotations[$depth]["incomplete"]) &&
                empty($annotations[$depth]["requires"]) &&
                empty($annotations[$depth]["skip"]) &&
                empty($annotations[$depth]["unstable"])) {
                continue;
            }

            // Go through each of the annotations and parse the incomplete annotation
            if (!empty($annotations[$depth]["incomplete"])) {
                foreach ($annotations[$depth]["incomplete"] as $annotation) {
                    $this->markTestIncomplete("Annotation @incomplete Found: "
                        . "Skipping {$this->toString()}");
                }
            }

            // Go through each of the annotations and parse the Cassandra/DSE version
            if (!empty($annotations[$depth]["requires"])) {
                foreach ($annotations[$depth]["requires"] as $annotation) {
                    // Ensure the server type is Cassandra or DSE and gather the requirements
                    $requires = explode(" ", $annotation);
                    $type = $requires[0];
                    if ($type == "Cassandra" || $type == "DSE" ||
                        $type == "cpp-driver" || $type == "cpp-driver-dse") {
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

                        // Determine if server type is being checked or driver
                        if ($type == "Cassandra" || $type == "DSE") {
                            // Get the version information from the configuration
                            $server_type = "Cassandra";
                            $server_version = self::$configuration->version;
                            if (self::$configuration->dse) {
                                if ($type == "Cassandra") {
                                    $server_version =
                                        self::$configuration->version->cassandra_version;
                                } else {
                                    $server_type = "DSE";
                                }
                            }

                            // Determine if the test should be skipped
                            if (!version_compare($server_version, $version, $operator)) {
                                $this->markTestSkipped("{$type} {$operator} v{$version} "
                                    . "is Required; {$server_type} v{$server_version} "
                                    . "is in Use: Skipping {$this->toString()}");
                            }
                        } else {
                            // Get the version information from the driver
                            $driver_version = Cassandra::CPP_DRIVER_VERSION;

                            // Determine if the driver type is valid the requirement
                            $driver_type = "C/C++";
                            if (class_exists("Dse")) {
                                if ($type == "cpp-driver-dse") {
                                    $driver_type .= " DSE";
                                    $driver_version = Dse::CPP_DRIVER_DSE_VERSION;
                                }
                            } else {
                                if ($type == "cpp-driver-dse") {
                                    continue;
                                }
                            }

                            // Determine if the test should be skipped
                            if (!version_compare($driver_version, $version, $operator)) {
                                $this->markTestSkipped("{$type} {$operator} v{$version} "
                                    . "is Required; {$driver_type} v{$driver_version} "
                                    . "is in Use: Skipping {$this->toString()}");
                            }
                        }
                    }
                }
            }

            // Go through each of the annotations and parse the skip annotation
            if (!empty($annotations[$depth]["skip"])) {
                foreach ($annotations[$depth]["skip"] as $reason) {
                    if (!empty(trim($reason))) {
                        $reason = " [{$reason}]";
                    }
                    $this->markTestSkipped("Annotation @skip Found: Skipping "
                        . $this->toString()
                        . $reason);
                }
            }

            // Go through each of the annotations and parse the unstable annotation
            if (!self::$configuration->unstable) {
                if (!empty($annotations[$depth]["unstable"])) {
                    foreach ($annotations[$depth]["unstable"] as $reason) {
                        if (!empty(trim($reason))) {
                            $reason = " [{$reason}]";
                        }
                        $this->markTestSkipped("Annotation @unstable Found: Skipping "
                            . $this->toString()
                            . $reason);
                    }
                }
            }
        }
    }

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Initialize the logger for the current test
        $this->initialize_logger();

        // Determine if the data center nodes should be initialized (default 1 node)
        if (count(self::$cluster_configuration->data_centers) == 0) {
            self::$cluster_configuration->add_data_center();
        }

        // Generate the keyspace name; apply unique id if keyspace is to long
        $this->keyspace = $this->get_short_name() . "_" . $this->getName(false);
        $this->keyspace = str_replace("test", "", strtolower($this->keyspace));
        if (strlen($this->keyspace) > self::KEYSPACE_MAXIMUM_LENGTH) {
            // Update the keyspace name with a unique ID
            $unique_id = uniqid();
            $this->keyspace = substr($this->keyspace,
                    0, self::KEYSPACE_MAXIMUM_LENGTH - strlen($unique_id)) .
                $unique_id;
        }

        // Generate the table name and filter out invalid characters
        $search = array(
            "org.apache.cassandra.db.marshal",
            "test",
            "with data set"
        );
        $this->table = preg_replace("/[^a-zA-Z0-9_]/", "_",
            str_replace($search, "",
                strtolower($this->getName($this->using_data_provider))));
        // Apply unique id if table is to long
        if (strlen($this->table) > self::TABLE_MAXIMUM_LENGTH) {
            // Update the table name with a unique ID
            $unique_id = uniqid();
            $this->table = substr($this->table,
                    0, self::TABLE_MAXIMUM_LENGTH - strlen($unique_id)) .
                $unique_id;
        }

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
                $this->establish_default_connection();
            }
        }
    }

    /**
     * @inheritdoc
     */
    protected function tearDown() {
        // Drop keyspace and close session for the integration test
        if (!is_null($this->session)) {
            try {
                $query = sprintf(self::DROP_KEYSPACE_FORMAT, $this->keyspace);
                $statement = new Cassandra\SimpleStatement($query);
                $this->session->execute($statement);
            } catch (\Exception $e) {
                ; // no-op
            }

            try {
                $this->session->close();
            } catch (\Exception $e) {
                ; // no-op
            }
        }

        // Close the log handle (if monitoring was started)
        if (!is_null($this->log_handle)) {
            fclose($this->log_handle);
        }

        // Reset the default cluster (in the event it was changed)
        self::$cluster_configuration = self::$ccm->default_cluster();
    }

    /**
     * Cassandra composite data types
     *
     * NOTE: Based on the version of Cassandra being used, only the proper
     *       composite data types will be returned
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function composite_data_types() {
        // Get the Cassandra version
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version) {
            $version = $version->cassandra_version;
        }

        // Get the composite data types that should be used
        $composite_data_types = $this->composite_data_types_1_2();
        if ($version->compare("2.1.0") >= 0) {
            $composite_data_types = array_merge(
                $composite_data_types,
                $this->composite_data_types_2_1()
            );
        }
        if ($version->compare("2.2.0") >= 0) {
            $composite_data_types = array_merge(
                $composite_data_types,
                $this->composite_data_types_2_2()
            );
        }

        // Return the composite data types
        return $composite_data_types;
    }

    /**
     * Data types (composite) - Cassandra v1.2
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 1.2.0
     */
    protected function composite_data_types_1_2() {
        // Define the composite data types
        $collection_type = Cassandra\Type::collection(Cassandra\Type::varchar());
        $map_type = Cassandra\Type::map(
            Cassandra\Type::int(),
            Cassandra\Type::varchar());
        $set_type = Cassandra\Type::set(Cassandra\Type::varchar());

        return array(
            // Collection data type
            array(
                $collection_type,
                array(
                    $collection_type->create("a", "b", "c"),
                    $collection_type->create("1", "2", "3"),
                    $collection_type->create(
                        "The quick brown fox jumps over the lazy dog",
                        "Hello World",
                        "DataStax PHP Driver Extension"
                    )
                )
            ),

            // Map data type
            array(
                $map_type,
                array(
                    $map_type->create(1, "a", 2, "b", 3, "c"),
                    $map_type->create(4, "1", 5, "2", 6, "3"),
                    $map_type->create(
                        7, "The quick brown fox jumps over the lazy dog",
                        8, "Hello World",
                        9, "DataStax PHP Driver Extension"
                    )
                )
            ),

            // Set data type
            array(
                $set_type,
                array(
                    $set_type->create("a", "b", "c"),
                    $set_type->create("1", "2", "3"),
                    $set_type->create(
                        "The quick brown fox jumps over the lazy dog",
                        "Hello World",
                        "DataStax PHP Driver Extension"
                    )
                )
            )
        );
    }

    /**
     * Data types (composite) - Cassandra v2.1
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 2.1.0
     */
    protected function composite_data_types_2_1() {
        // Define the tuple data type
        $tuple_type = Cassandra\Type::tuple(
            Cassandra\Type::bigint(),
            Cassandra\Type::int(),
            Cassandra\Type::varchar()
        );

        return array(
            // Tuple data type
            array(
                $tuple_type,
                array(
                    $tuple_type->create(
                        new Cassandra\Bigint(1),
                        2,
                        "a"
                    ),
                    $tuple_type->create(
                        new Cassandra\Bigint(3),
                        4,
                        "b"
                    ),
                    $tuple_type->create(
                        new Cassandra\Bigint(5),
                        6,
                        "c"
                    )
                )
            )
        );
    }

    /**
     * Data types (composite) - Cassandra v2.2
     *
     * @return array Composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 2.2.0
     */
    protected function composite_data_types_2_2() {
        // Define the user data type
        $user_data_type_type = Cassandra\Type::userType(
            "a", Cassandra\Type::bigint(),
            "b", Cassandra\Type::int(),
            "c", Cassandra\Type::varchar()
        );
        $name = $this->generate_user_data_type_name($user_data_type_type);
        $user_data_type_type = $user_data_type_type->withName($name);

        return array(
            // User data type
            array(
                $user_data_type_type,
                array(
                    $user_data_type_type->create(
                        "a", new Cassandra\Bigint(1),
                        "b", 2,
                        "c", "x"
                    ),
                    $user_data_type_type->create(
                        "a", new Cassandra\Bigint(3),
                        "b", 4,
                        "c", "y"
                    ),
                    $user_data_type_type->create(
                        "a", new Cassandra\Bigint(5),
                        "b", 6,
                        "c", "z"
                    )
                )
            )
        );
    }

    /**
     * Connect to the Cassandra/DSE cluster and get the version information from
     * the server (may be different than the configuration settings version
     * used)
     *
     * @param Cassandra\Session|null $cluster (Optional) Cluster instance
     *                                                   (default: base cluster
     *                                                   instance)
     * @param string|null $query (Optional) Query to execute upon connection
     *                                      (default: generated create keyspace
     *                                                query)
     * @return array Session and server version from established connection
     *     [
     *         "rows"  => (Cassandra\Rows) Query result (rows)
     *         "session" => (Cassandra\Session) Session instance
     *         "version" => (Cassandra\Version|Dse\Version) Server version
     *     ]
     *
     * @see Cassandra\Version
     * @see Dse\Version
     */
    protected function connect_cluster($cluster = null, $query = null) {
        // Establish the connection to the server
        if (is_null($cluster)) {
            $cluster = $this->cluster;
        }
        try {
            $session = $cluster->connect();
        } catch (\Exception $e) {
            echo $e->getMessage() . PHP_EOL;
        }

        // Get the server version
        if (self::$configuration->dse) {
            $version = Dse\Version::from_server($session);
        } else {
            $version = CassandraVersion::from_server($session);
        }

        // Create the keyspace
        if (is_null($query)) {
            $query = $this->create_keyspace_query;
        }
        if (self::$configuration->verbose) {
            echo "Executing Query: {$query}" . PHP_EOL;
        }
        $statement = new Cassandra\SimpleStatement($query);
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
     * @param CCM\Cluster|null $cluster_configuration (Optional) CCM cluster
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
    protected function create_server_cluster(CCM\Cluster $cluster_configuration = null,
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
     * Create the given UDT on the server
     *
     * @param $user_data_type UDT to create type for
     * @param string|null $keyspace (Optional) Keyspace to create UDT in
     */
    protected function create_user_data_type($user_data_type, $keyspace = null) {
        // Determine if the default keyspace name should be used
        if (is_null($keyspace)) {
            $keyspace = $this->keyspace;
        }

        // Generate the UDT fields that make up the CQL type
        $udt_fields = implode(", ", array_map(
            function ($key, $data_type) {
                return "{$key} " . $this->generate_cql_data_type($data_type, true);
            },
            array_keys($user_data_type->types()),
            $user_data_type->types())
        );

        // Create and execute the query
        $query = sprintf(self::CREATE_TYPE_FORMAT,
            $keyspace,
            $this->generate_user_data_type_name($user_data_type),
            $udt_fields);
        if (self::$configuration->verbose) {
            echo "Executing Query: {$query}" . PHP_EOL;
        }
        $this->session->execute(new Cassandra\SimpleStatement($query));
    }

    /**
     * Cassandra data types
     *
     * NOTE: Based on the version of Cassandra being used, only the proper
     *       data types will be returned
     *
     * @param bool $primary_keys (Optional) True if data types will be used as
     *                                      a primary key; false otherwise
     *                                      (default: false)
     * @return array Composite, nested composite and scalar data type to use in
     *               a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function data_types($primary_keys = false) {
        // Get the Cassandra version
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version) {
            $version = $version->cassandra_version;
        }

        // Return all the valid composite and scalar data types for Cassandra version
        $data_types = $this->scalar_data_types_1_2();
        if (!$primary_keys) {
            $data_types = array_merge(
                $data_types,
                $this->composite_data_types_1_2(),
                $this->nested_composite_data_types_1_2()
            );
        }
        if ($version->compare("2.1.0") >= 0) {
            $data_types = array_merge(
                $data_types,
                $this->composite_data_types_2_1(),
                $this->nested_composite_data_types_2_1()
            );
        }
        if ($version->compare("2.2.0") >= 0) {
            $data_types = array_merge(
                $data_types,
                $this->composite_data_types_2_2(),
                $this->nested_composite_data_types_2_2()
            );
        }
        if ($version->compare("2.2.3") >= 0) {
            $data_types = array_merge(
                $data_types,
                $this->scalar_data_types_2_2_3()
            );
        }
        if (!$primary_keys && $version->compare("3.10") >= 0) {
            $data_types = array_merge(
                $data_types,
                $this->scalar_data_types_3_10()
            );
        }
        return $data_types;
    }

    /**
     * Get the default cluster builder
     *
     * @param bool $all (Optional) True if all contact should be used; false
     *                             otherwise (e.g. liveset) (default: true)
     * @return Cassandra\Cluster\Builder Cluster builder instance
     *
     * @see Cassandra\Cluster\Builder
     * @see Dse\Cluster\Builder
     */
    protected function default_cluster_builder($all = true) {
        // Get the contact points from CCM
        $contact_points = self::$ccm->contact_points($all);

        // Assign the defaults for the cluster configuration
        $cluster = Cassandra::cluster()->withContactPoints($contact_points)
            ->withPersistentSessions(true)
            ->withRandomizedContactPoints(false)
            ->withSchemaMetadata(false);
        return $cluster;
    }

    /**
     * Get the default cluster
     *
     * @param bool $all (Optional) True if all contact should be used; false
     *                             otherwise (e.g. liveset) (default: true)
     * @return Cassandra\Cluster Cluster instance
     */
    protected function default_cluster($all = true) {
        return $this->default_cluster_builder($all)
            ->build();
    }

    /**
     * Establish the connection; assigns cluster, session, and server
     * version
     *
     * @param Cassandra\Cluster Cluster instance
     *
     * @see IntegrationTest::cluster
     * @see IntegrationTest::server_version
     * @see IntegrationTest::session
     */
    protected function establish_connection($cluster) {
        $this->cluster = $cluster;
        $connection = $this->connect_cluster($cluster);
        $this->session = $connection["session"];
        $this->server_version = $connection["version"];
    }

    /**
     * Establish the default connection; assigns cluster, session, and server
     * version
     *
     * @see IntegrationTest::cluster
     * @see IntegrationTest::server_version
     * @see IntegrationTest::session
     */
    protected function establish_default_connection() {
        $this->establish_connection($this->default_cluster());
    }

    /**
     * Generate the CQL data type from the give data type
     *
     * @param Cassandra\Type $type Data type
     * @return string String representation of the CQL data type
     */
    protected function generate_cql_data_type($type) {
        // Determine if the data type is a composite value and should be frozen
        if ($this->is_composite_data_type($type)) {
            // Ensure that UDTs are frozen
            if ($type instanceof Cassandra\Type\UserType) {
                return sprintf("frozen<%s>", (string) $type);
            }

            // Gather the value types from the composite type
            $types = array();
            if ($type instanceof Cassandra\Type\Collection ||
                $type instanceof Cassandra\Type\Set) {
                $types[] = $type->valueType();
            } else if ($type instanceof Cassandra\Type\Map) {
                $types[] = $type->keyType();
                $types[] = $type->valueType();
            } else if ($type instanceof Cassandra\Type\Tuple) {
                $types = $type->types();
            }

            // Freeze the interior composite data types
            $cql_data_type = implode(", ",
                array_map(
                    function ($value) {
                        if ($this->is_composite_data_type($value)) {
                            return sprintf("frozen<%s>", (string) $value);
                        } else {
                            return (string) $value;
                        }
                    },
                    $types
                )
            );
            $cql_data_type = sprintf("{$type->name()}<%s>", $cql_data_type);
            return $cql_data_type;
        }

        // Normal data type (return the string representation)
        return (string) $type;
    }

    /**
     * Determine if a given type is a composite data type
     *
     * @param Cassandra\Type $type Type to determine if it is a composite
     * @return bool True if $type is a composite data type; false otherwise
     */
    protected function is_composite_data_type($type) {
        if ($type instanceof Cassandra\Type\Collection ||
            $type instanceof Cassandra\Type\Map ||
            $type instanceof Cassandra\Type\Set ||
            $type instanceof Cassandra\Type\Tuple ||
            $type instanceof Cassandra\Type\UserType) {
            return true;
        }
        return false;
    }

    /**
     * Get the contents of the test's log file
     *
     * NOTE: If the log monitoring was not started then the entire contents of
     *       of the log file will be returned
     *
     * @return string Contents of the test's log file
     *
     * @see IntegrationTest::start_log_monitoring()
     */
    protected function log_contents() {
        if (is_null($this->log_handle)) {
            $this->log_handle = fopen($this->log_directory . $this->log_filename, "r");
        }

        return stream_get_contents($this->log_handle);
    }

    /**
     * Cassandra nested composite data types
     *
     * NOTE: Based on the version of Cassandra being used, only the proper
     *       nested composite data types will be returned
     *
     * @return array Nested composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function nested_composite_data_types() {
        // Get the Cassandra version
        $version = IntegrationTestFixture::get_instance()->configuration->version;
        if ($version instanceof Dse\Version) {
            $version = $version->cassandra_version;
        }

        // Get the nested composite data types that should be used
        $nested_composite_data_types = $this->nested_composite_data_types_1_2();
        if ($version->compare("2.1.0") >= 0) {
            $nested_composite_data_types = array_merge(
                $nested_composite_data_types,
                $this->nested_composite_data_types_2_1()
            );
        }
        if ($version->compare("2.2.0") >= 0) {
            $nested_composite_data_types = array_merge(
                $nested_composite_data_types,
                $this->nested_composite_data_types_2_2()
            );
        }

        // Return the composite data types
        return $nested_composite_data_types;
    }

    /**
     * Nested Data types (composite) - Cassandra v1.2
     *
     * @return array Nested composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 1.2.0
     */
    protected function nested_composite_data_types_1_2() {
        // Get the composite data types that should be nested
        $composite_data_types = $this->composite_data_types();

        // Create the nested composite (collection)
        $nested_composite_data_types = array();
        foreach ($composite_data_types as $composite_data_type) {
            // Define and create the nested composite data type (one value only)
            $type = Cassandra\Type::collection($composite_data_type[0]);
            $nested_composite_data_types[] = array(
                $type,
                array(
                    $type->create($composite_data_type[1][0])
                )
            );
        }

        // Create the nested composite (map)
        foreach ($composite_data_types as $composite_data_type) {
            // Define and create the nested composite data type (one value only)
            $type = Cassandra\Type::map($composite_data_type[0], $composite_data_type[0]);
            $nested_composite_data_types[] = array(
                $type,
                array(
                    $type->create($composite_data_type[1][0], $composite_data_type[1][1])
                )
            );
        }

        // Create the nested composite (set)
        foreach ($composite_data_types as $composite_data_type) {
            // Define and create the nested composite data type (one value only)
            $type = Cassandra\Type::set($composite_data_type[0]);
            $nested_composite_data_types[] = array(
                $type,
                array(
                    $type->create($composite_data_type[1][0])
                )
            );
        }

        // Return the nested composite data types
        return $nested_composite_data_types;
    }

    /**
     * Nested Data types (composite) - Cassandra v2.1
     *
     * @return array Nested composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 2.1.0
     */
    protected function nested_composite_data_types_2_1() {
        // Get the composite data types that should be nested
        $composite_data_types = $this->composite_data_types();

        // Create the nested composite (tuple)
        foreach ($composite_data_types as $composite_data_type) {
            // Define and create the nested composite data type (one value only)
            $type = Cassandra\Type::tuple($composite_data_type[0], $composite_data_type[0]);
            $nested_composite_data_types[] = array(
                $type,
                array(
                    $type->create($composite_data_type[1][0], $composite_data_type[1][1])
                )
            );
        }

        // Return the nested composite data types
        return $nested_composite_data_types;
    }

    /**
     * Nested Data types (composite) - Cassandra v2.2
     *
     * @return array Nested composite data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 2.2.0
     */
    protected function nested_composite_data_types_2_2() {
        // Get the composite data types that should be nested
        $composite_data_types = $this->composite_data_types();

        // Create the nested composite (UDT)
        foreach ($composite_data_types as $composite_data_type) {
            // Define the user data type
            $type = Cassandra\Type::userType(
                "y", $composite_data_type[0],
                "z", $composite_data_type[0]
            );
            $name = $this->generate_user_data_type_name($type);
            $type = $type->withName($name);

            // Create the nested composite data type (one value only)
            $nested_composite_data_types[] = array(
                $type,
                array(
                    $type->create(
                        "y", $composite_data_type[1][0],
                        "z", $composite_data_type[1][1]
                    )
                )
            );
        }

        // Return the nested composite data types
        return $nested_composite_data_types;
    }

    /**
     * Cassandra data types that can be used as primary keys
     *
     * NOTE: Based on the version of Cassandra being used, only the proper
     *       data types will be returned
     *
     * @return array Composite and scalar data type for use as a primary key to
     *               use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     */
    protected function primary_data_types() {
        return $this->data_types(true);
    }

    /**
     * Data types (scalar) - Cassandra v1.2
     *
     * @return array Scalar data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 1.2.0
     */
    protected function scalar_data_types_1_2() {
        return array(
            // ASCII data type
            array(
                Cassandra\Type::ascii(),
                array(
                    "a",
                    "b",
                    "c",
                    "1",
                    "2",
                    "3"
                )
            ),

            // Bigint data type
            array(
                Cassandra\Type::bigint(),
                array(
                    new Cassandra\Bigint(PHP_INT_MAX),
                    new Cassandra\Bigint(self::PHP_INT_MIN),
                    new Cassandra\Bigint("0"),
                    new Cassandra\Bigint("37")
                )
            ),

            // Blob data type
            array(
                Cassandra\Type::blob(),
                array(
                    new Cassandra\Blob("DataStax PHP Driver Extension"),
                    new Cassandra\Blob("Cassandra"),
                    new Cassandra\Blob("DataStax Enterprise")
                )
            ),

            // Boolean data type
            array(
                Cassandra\Type::boolean(),
                array(
                    true,
                    false
                )
            ),

            //TODO: Should we add a counter data type?

            // Decimal data type
            array(
                Cassandra\Type::decimal(),
                array(
                    new Cassandra\Decimal("3.14159265358979323846"),
                    new Cassandra\Decimal("2.71828182845904523536"),
                    new Cassandra\Decimal("1.61803398874989484820")
                )
            ),

            // Double data type
            array(
                Cassandra\Type::double(),
                array(
                    3.1415926535,
                    2.7182818284,
                    1.6180339887
                )
            ),

            // Float data type
            array(
                Cassandra\Type::float(),
                array(
                    new Cassandra\Float(3.14159),
                    new Cassandra\Float(2.71828),
                    new Cassandra\Float(1.61803)
                )
            ),

            // Inet data type
            array(
                Cassandra\Type::inet(),
                array(
                    new Cassandra\Inet("127.0.0.1"),
                    new Cassandra\Inet("0:0:0:0:0:0:0:1"),
                    new Cassandra\Inet("2001:db8:85a3:0:0:8a2e:370:7334")
                )
            ),

            // Integer data type
            array(
                Cassandra\Type::int(),
                array(
                    2147483647,
                    -2147483647 - 1, // Operation of INT32_MAX performed due to PHP v5.x edge case (casts to double and fails insert)
                    0,
                    148
                )
            ),

            // Text data type
            array(
                Cassandra\Type::text(),
                array(
                    "The quick brown fox jumps over the lazy dog",
                    "Hello World",
                    "DataStax PHP Driver Extension"
                )
            ),

            // Timestamp data type
            array(
                Cassandra\Type::timestamp(),
                array(
                    new Cassandra\Timestamp(123),
                    new Cassandra\Timestamp(456),
                    new Cassandra\Timestamp(789),
                    new Cassandra\Timestamp(time())
                )
            ),

            // Timeuuid data type
            array(
                Cassandra\Type::timeuuid(),
                array(
                    new Cassandra\Timeuuid(0),
                    new Cassandra\Timeuuid(1),
                    new Cassandra\Timeuuid(2),
                    new Cassandra\Timeuuid(time())
                )
            ),

            // Uuid data type
            array(
                Cassandra\Type::uuid(),
                array(
                    new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"),
                    new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c3"),
                    new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c4")
                )
            ),

            // Varchar data type
            array(
                Cassandra\Type::varchar(),
                array(
                    "The quick brown fox jumps over the lazy dog",
                    "Hello World",
                    "DataStax PHP DSE Driver Extension"
                )
            ),

            // Varint data type
            array(
                Cassandra\Type::varint(),
                array(
                    new Cassandra\Varint(2147483647),
                    new Cassandra\Varint(-2147483648),
                    new Cassandra\Varint(0),
                    new Cassandra\Varint(296)
                )
            )
        );
    }

    /**
     * Data types (scalar) - Cassandra v2.2.3
     *
     * @return array Scalar data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 2.2.3
     */
    protected function scalar_data_types_2_2_3() {
        return array(
            // Date data type
            array(
                Cassandra\Type::date(),
                array(
                    new Cassandra\Date(),
                    new Cassandra\Date(0),
                    new Cassandra\Date(-86400),
                    new Cassandra\Date(86400)
                )
            ),

            // Smallint data type
            array(
                Cassandra\Type::smallint(),
                array(
                    Cassandra\Smallint::min(),
                    Cassandra\Smallint::max(),
                    new Cassandra\Smallint(0),
                    new Cassandra\Smallint(74)
                )
            ),

            // Time data type
            array(
                Cassandra\Type::time(),
                array(
                    new Cassandra\Time(),
                    new Cassandra\Time(0),
                    new Cassandra\Time(1234567890)
                )
            ),

            // Tinyint data type
            array(
                Cassandra\Type::tinyint(),
                array(
                    Cassandra\Tinyint::min(),
                    Cassandra\Tinyint::max(),
                    new Cassandra\Tinyint(0),
                    new Cassandra\Tinyint(37)
                )
            )
        );
    }

/**
     * Data types (scalar) - Cassandra v3.10
     *
     * @return array Scalar data type to use in a data provider
     *     [
     *         [0] => (Cassandra\Type) Data type
     *         [1] => (array) Array of data type values
     *     ]
     *
     * @requires Cassandra >= 3.10
     */
    protected function scalar_data_types_3_10() {
        return array(
            // Duration data type
            array(
                Cassandra\Type::duration(),
                array(
                    new Cassandra\Duration(1, 2, 3),
                    new Cassandra\Duration(1, 0, Cassandra\Bigint::max()),
                    new Cassandra\Duration(-1, 0, Cassandra\Bigint::min()),
                    new Cassandra\Duration((2 ** 31) - 1, 1, 0),
                    new Cassandra\Duration(-(2 ** 31), -1, 0),
                    new Cassandra\Duration(0, (2 ** 31) - 1, 1),
                    new Cassandra\Duration(0, -(2 ** 31), -1)
                )
            )
        );
    }

    /**
     * Select all rows from a given keyspace and table
     *
     * @param string|null $keyspace (Optional) Keyspace to select from
     * @param string|null $table (Optional) Table to select from
     * @return Cassandra\Rows Select statement result
     */
    protected function select_all_rows($keyspace = null, $table = null) {
        // Determine if the base keyspace and table should be used
        if (is_null($keyspace)) {
            $keyspace = $this->keyspace;
        }
        if (is_null($table)) {
            $table = $this->table;
        }

        // Create and execute the select query
        $query = sprintf(self::SELECT_FORMAT, $keyspace, $table);
        $statement = new Cassandra\SimpleStatement($query);
        if (self::$configuration->verbose) {
            echo "Executing Query: {$query}" . PHP_EOL;
        }
        return $this->session->execute($statement);
    }

    /**
     * Start the log monitoring; open the test's log file for reading and seek
     * to the end of the file.
     */
    protected function start_log_monitoring() {
        // Open the file for reading and seek to the end of the file
        if (is_null($this->log_handle)) {
            $this->log_handle = fopen($this->log_directory . $this->log_filename, "r");
        }
        fseek($this->log_handle, -1, SEEK_END);
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
    protected function start_server_cluster($jvm_arguments = array()) {
        // Start the cluster and assert that the cluster is ready
        PHPUnit_Framework_Assert::assertTrue(self::$ccm->start_cluster(
            array_key_exists("jvm", $jvm_arguments)
                ? $jvm_arguments["jvm"]
                : array()
        ), (self::$configuration->dse ? "DSE" : "Cassandra")
            . " cluster was not started properly");
    }

    /**
     * Determine if a node is available in teh active cluster
     *
     * @param int $node Node to check availability
     * @return bool True if node is available; false otherwise
     */
    protected function wait_for_port_on_node($node, $port) {
        // Create the remote socket
        $ip_address = self::$ccm->ip_addresses()[$node - 1];
        $remote_socket = "tcp://{$ip_address}:{$port}";

        // Attempt to establish connection (timeout = 1s)
        $connection = @stream_socket_client("{$remote_socket}",
            $error_code, $error_message, 1);
        if ($connection) {
            fclose($connection);
            return true;
        }
        return false;
    }

    /**
     * Generate a user type name (string) suitable for creating a table or CQL
     * UDT
     *
     * @param Cassandra\Type\UserType $user_data_type UDT to generate name
     * @return string String representation of the UDT
     */
    protected function generate_user_data_type_name($user_data_type) {
        $name = implode("_", array_map(
            function ($key, $data_type) {
                $filter = preg_replace("/[^a-zA-Z0-9_]/", "",
                    $this->generate_cql_data_type($data_type));
                $filter = preg_replace("/(_)\\1+/", "_", $filter);
                return $key . $filter;
            },
            array_keys($user_data_type->types()),
            $user_data_type->types())
        );
        return $name;
    }

    /**
     * Get the short name of the class without the namespacing.
     *
     * @return string Short name for the class name
     */
    private function get_short_name() {
        $class = new \ReflectionClass($this);
        $search = array("org.apache.cassandra.db.marshal.");
        return str_replace($search, "", $class->name);
    }

    /**
     * Initialize/Alter the driver logger configuration to force each test to
     * utilize its own log file and force the maximum log level
     */
    private function initialize_logger() {
        // Create the directory for the test class (directory may already exist)
        $this->log_directory = "logs" . DIRECTORY_SEPARATOR
            . $this->get_short_name() . DIRECTORY_SEPARATOR;
        if ($this->using_data_provider) {
            $this->log_directory .= $this->getName(false) . DIRECTORY_SEPARATOR;
        }
        @mkdir($this->log_directory, 0777, true);

        // Determine the logger filename
        $this->log_filename = "{$this->getName(false)}.log";
        if ($this->using_data_provider) {
            // Determine if we can retrieve the name of the data set
            $dataset_name = $this->getDataSetAsString(false);
            if (preg_match('/"([^"]+)"/', $dataset_name, $match)) {
                $dataset_name = $match[1];
            }
            $this->log_filename = "{$dataset_name}.log";
        }

        // Simplify the filename and conform to proper file conventions
        $this->log_filename = str_replace(" ", "", $this->log_filename);
        $this->log_filename = str_replace(array("<", ">", ",", "#"),
            "_", $this->log_filename);
        $this->log_filename = preg_replace("/(.)\\1+/", "_", $this->log_filename);

        // Update the logger filename and level (alter INI)
        ini_alter(LOG_FILENAME_OPTION, "{$this->log_directory}{$this->log_filename}");
        ini_alter(LOG_LEVEL_OPTION, "TRACE");
    }
}
