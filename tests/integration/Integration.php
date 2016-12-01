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
class IntegrationFixture {
    /**
     * @var IntegrationFixture Singleton instance for the fixture
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
            self::$instance  = new IntegrationFixture();
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
            . "  Cassandra driver v" . \Cassandra::VERSION . PHP_EOL
            . "  DSE driver v" . \Dse::VERSION . PHP_EOL
            . "  C/C++ DSE drvier v" . \Dse::CPP_DRIVER_DSE_VERSION . PHP_EOL;
        $this->configuration->print_settings();

        // Create the CCM instance for the entire run of the integration tests
        $this->ccm = new CCM\Bridge($this->configuration);

        // Clear any clusters that may have been left over from previous tests
        $this->ccm->remove_clusters();
    }
}

/**
 * Base class to provide common integration test functionality.
 */
abstract class Integration extends \PHPUnit_Framework_TestCase {
    /**
     * Maximum length for the keyspace (server limit)
     */
    const KEYSPACE_MAXIMUM_LENGTH = 48;

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
     * @var bool True if session should be established; false otherwise
     */
    protected $create_session = true;
    /**
     * @var string Keyspace to use for the test(s)
     */
    protected $keyspace;
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
     * @inheritdoc
     */
    public static function setUpBeforeClass() {
        $fixture = IntegrationFixture::get_instance();

        // Get the CCM global instance and create the initial cluster
        self::$ccm = $fixture->ccm;
        self::$cluster_configuration = self::$ccm->default_cluster();
        self::$cluster_configuration->add_data_center();

        // Assign the configuration from the global instance
        self::$configuration = $fixture->configuration;
    }

    /**
     * @inheritdoc
     */
    protected function setUp() {
        // Create the keyspace name; apply unique id if keyspace is to long
        $this->keyspace = $this->get_short_name() . "_" . $this->getName(false);
        if (strlen($this->keyspace) > self::KEYSPACE_MAXIMUM_LENGTH) {
            // Update the keyspace name with a unique ID
            $unique_id = uniqid();
            $this->keyspace = substr($this->keyspace,
                    0, self::KEYSPACE_MAXIMUM_LENGTH - strlen($unique_id)) .
                $unique_id;
        }
        $this->keyspace = strtolower($this->keyspace);

        // Create and start the CCM cluster
        self::$ccm->create_cluster(self::$cluster_configuration);
        if ($this->start_cluster) {
            if (self::$ccm->start_cluster()) {
                if ($this->create_session) {
                    // Establish the session connection
                    $this->cluster = $this->default_cluster();
                    $this->session = $this->cluster->connect();

                    // Get the server version
                    if (self::$configuration->dse) {
                        $this->server_version = Dse\Version::from_server($this->session);
                    } else {
                        $this->server_version = Cassandra\Version::from_server($this->session);
                    }
                }
            } else {
                throw new \Exception("Cluster was not started properly");
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
                $statement = new \Cassandra\SimpleStatement('DROP KEYSPACE "' . $this->keyspace . '"');
                $this->session->execute($statement);
            } catch (\Exception $e) {
                ; // no-op
            }
        }

        // Reset the default cluster (in the event it was changed)
        self::$cluster_configuration = self::$ccm->default_cluster();
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
        $class = new \ReflectionClass(get_class());
        return $class->getShortName();
    }
}