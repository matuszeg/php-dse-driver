<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

namespace CCM {
    /**
     * Default cluster prefix
     */
    const DEFAULT_CLUSTER_PREFIX = "php-driver";

    /**
     * Binary port for CQL client connections
     */
    const BINARY_PORT = 9042;
    /**
     * Maximum number of retries when checking the cluster/node up/down status
     */
    const MAX_RETRIES = 100;
    /**
     * The amount of time to sleep when checking the cluster/node status
     */
    const NAP = 100000; // 100ms (0.1s)

/**
 * This class contains the allowed workloads for a DSE cluster.
 *
 * @package CCM
 */
class Workload {
    /**
     * Cassandra
     */
    const CASSANDRA = "cassandra";
    /**
     * CFS - Cassandra file system (Hadoop Distributed File System (HDFS)
     * replacement
     */
    const CFS = "cfs";
    /**
     * DSEFS - DataStax Enterprise file system (Spark streaming and Write Ahead
     * Logging (WAL))
     */
    const DSEFS = "dsefs";
    /**
     * Graph
     */
    const GRAPH = "graph";
    /**
     * Hadoop
     */
    const HADOOP = "hadoop";
    /**
     * Solr
     */
    const SOLR = "solr";
    /**
     * Spark
     */
    const SPARK = "spark";
}

/**
 * This class describes the settings of a Cassandra/DSE cluster.
 *
 * @package CCM
 */
class Cluster {
    /**
     * @var bool True if client authentication is enabled; false otherwise
     */
    private $client_authentication = false;
    /**
     * @var array Number of nodes for each data center in the cluster
     */
    private $data_centers;
    /**
     * @var bool True if this cluster will be a DSE cluster; false otherwise
     */
    private $dse;
    /**
     * @var bool True if the DSE INI credentials file should be used for
     *                download authentication; false otherwise
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
     * @var string Prefix to use when creating a cluster name; default
     *             DEFAULT_CLUSTER_PREFIX
     */
    private $prefix;
    /**
     * @var bool True if SSL should be enabled; false otherwise
     */
    private $ssl = false;
    /**
     * @var mixed Cassandra/DSE version to use
     *
     * @see \Cassandra\Version
     * @see \Dse\Version
     */
    private $version;
    /**
     * @var bool True if vnode tokens should be used; false otherwise
     */
    private $vnodes = false;
    /**
     * @var array DSE workload to utilize
     */
    private $workload;

    /**
     * Build a cluster instance
     *
     * @return Cluster Cluster instance for builder object
     */
    public static function build() {
        return new Cluster(false, false, null, null, null, DEFAULT_CLUSTER_PREFIX);
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            if ($property == "workload") {
                if (!isset($this->workload)) {
                    return array(Workload::CASSANDRA);
                }
            }
            return $this->$property;
        }
    }

    public function __toString() {
        // Generate the cluster name from the settings of the cluster instance
        $name = "{$this->prefix}_{$this->version}_"
            . implode("-", $this->data_centers);
        if ($this->vnodes) {
            $name .= "-vnodes";
        }
        if ($this->ssl) {
            $name .= "-ssl";
            if ($this->client_authentication) {
                $name .= "-client_authentication";
            }
        }
        if (isset($this->workload)) {
            $workloads = array_diff($this->workload, array(Workload::CASSANDRA));
            if (!empty($workloads)) {
                $name .= "-" . implode("-", $workloads);
            }
        }
        return $name;
    }

    /**
     * Add a data center with the given number of nodes
     *
     * @param int $nodes (Optional) Number of nodes to add for data center
     *                             (default: 1)
     * @return $this Cluster
     */
    public function add_data_center($nodes = 1) {
        $this->data_centers[] = $nodes;
        return $this;
    }

    /**
     * Add a DSE workload to apply to the cluster
     *
     * @param string $workload DSE workload to add
     * @return $this Cluster
     */
    public function add_workload($workload) {
        $this->workload[] = $workload;
        return $this;
    }

    /**
     * Enable/Disable client authentication
     *
     * @param bool $enable (Optional) True to enable; false otherwise
     *                                (default: true)
     * @return $this Cluster
     */
    public function with_client_authentication($enable = true) {
        $this->client_authentication = $enable;
        return $this;
    }

    /**
     * Enable/Disable DSE
     *
     * @param bool $enable (Optional) True to enable; false otherwise
     *                                (default: true)
     * @return $this Cluster
     */
    public function with_dse($enable = true) {
        $this->dse = $enable;
        $this->update_version($this->version);
        return $this;
    }

    /**
     * Enable/Disable DSE INI credentials for download authentication
     *
     * NOTE: This will override username/password credentials
     *
     * @param bool $enable (Optional) True to enable; false otherwise
     *                                (default: true)
     * @return $this Cluster
     */
    public function with_dse_ini($enable = true) {
        $this->dse_ini = $enable;
        return $this;
    }

    /**
     * DSE username for download authentication
     *
     * @param string $username DSE username
     * @return $this Cluster
     */
    public function with_dse_username($username) {
        $this->dse_username = $username;
        return $this;
    }

    /**
     * DSE password for download authentication
     *
     * @param string $password DSE password
     * @return $this Cluster
     */
    public function with_dse_password($password) {
        $this->dse_password = $password;
        return $this;
    }

    /**
     * Prefix to utilize for cluster
     *
     * @param string $prefix Prefix for cluster
     * @return $this Cluster
     */
    public function with_prefix($prefix) {
        $this->prefix = $prefix;
        return $this;
    }

    /**
     * Enable/Disable SSL
     *
     * @param bool $enable (Optional) True to enable; false otherwise
     *                                (default: true)
     * @return $this Cluster
     */
    public function with_ssl($enable = true) {
        $this->ssl = $enable;
        return $this;
    }

    /**
     * Cassandra/DSE version
     *
     * @param mixed $version Cassandra/DSE version to use
     * @return $this Cluster
     */
    public function with_version($version) {
        $this->update_version($version);
        return $this;
    }

    /**
     * Enable/Disable vnodes
     *
     * @param bool $enable (Optional) True to enable; false otherwise
     *                                (default: true)
     * @return $this Cluster
     */
    public function with_vnodes($enable = true) {
        $this->vnodes = $enable;
        return $this;
    }

    /**
     * Build a cluster instance
     *
     * @param bool $dse True if DSE is requested; false otherwise
     * @param bool $dse_ini True if DSE INI credentials is requested; false
     *                      otherwise
     * @param string $dse_username Username for DSE download authentication
     * @param string $dse_password Password for DSE download authentication
     * @param mixed $version Cassandra/DSE version to use
     * @param string $prefix Prefix to use when creating a cluster name
     * @return Cluster Cluster instance
     */
    private function __construct($dse, $dse_ini, $dse_username, $dse_password, $version, $prefix) {
        $this->data_centers = array();
        $this->dse = $dse;
        if ($dse) {
            $this->dse_ini = $dse_ini;
            $this->dse_username = $dse_username;
            $this->dse_password = $dse_password;
        }
        $this->prefix = $prefix;
        if (is_null($version)) {
            if ($dse) {
                $this->version = new \Dse\Version(\Configuration::DEFAULT_DSE_VERSION);
            } else {
                $this->version = new \Cassandra\Version(\Configuration::DEFAULT_CASSANDRA_VERSION);
            }
        } else {
            $this->version = $version;
        }
    }

    /**
     * Internal update for the Cassandra/DSE version being used
     *
     * NOTE: This is used when $dse is updated and $version is assigned
     *
     * @param mixed $version Cassandra/DSE version to update
     */
    private function update_version($version) {
        if (is_string($version)) {
            if ($this->dse) {
                $version = new \Dse\Version($version);
            } else {
                $version = new \Cassandra\Version($version);
            }
        }
        $this->version = $version;
    }
}

/**
 * This class provides an interface to the external CCM command which allows
 * for quickly setting up and tearing down Cassandra/DSE clusters for testing.
 *
 * @package CCM
 */
class Bridge {
    /**
     * Timeout for CCM process (10 minutes)
     */
    const PROCESS_TIMEOUT = 600;

    /**
     * @var bool True if this cluster will be a DSE cluster; false otherwise
     */
    private $dse = false;
    /**
     * @var bool|string DSE ini filename if using SSH deployments; true to
     *                  utilize local INI credentials file
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
     * @var string EOL marker expected in all CCM command outputs
     */
    private $eol = "\n";
    /**
     * @var string Prefix to use when creating a cluster name
     */
    private $prefix;
    /**
     * @var resource SSH session for remote deployment
     */
    private $ssh = null;
    /**
     * @var string Host/IPv4 address for SSH session
     */
    private $ssh_host;
    /**
     * @var mixed Cassandra/DSE version to use
     *
     * @see \Cassandra\Version
     * @see \Dse\Version
     */
    private $version;
    /**
     * @var bool True if verbose output should be enable; false otherwise
     */
    private $verbose = false;
    //TODO: Add the ability to use branch/tags and local installation directory

    /**
     * Bridge constructor
     *
     * @param \Configuration|null $configuration (Optional) Configuration settings
     *                                                      to apply to the CCM
     *                                                      bridge
     * @throws \Exception If remote deployment is enabled and connection cannot
     *                    be established
     */
    public function __construct(\Configuration $configuration = null) {
        if (!is_null($configuration)) {
            $this->dse = $configuration->dse;
            $this->dse_ini = $configuration->dse_ini;
            $this->dse_username = $configuration->dse_username;
            $this->dse_password = $configuration->dse_password;
            $this->prefix = $configuration->prefix;
            $this->version = $configuration->version;
            $this->verbose = $configuration->verbose;

            // Determine if the deployment type is SSH
            if ($configuration->ssh_host) {
                if (function_exists("ssh2_connect")) {
                    $this->ssh = ssh2_connect($configuration->ssh_host,
                        $configuration->ssh_port);
                    if ($this->ssh) {
                        $this->ssh_host = $configuration->ssh_host;

                        // Determine if public/private key or password authentication
                        if ($configuration->ssh_public_key &&
                            $configuration->ssh_private_key) {
                            if (!ssh2_auth_pubkey_file($this->ssh,
                                $configuration->ssh_username,
                                $configuration->ssh_public_key,
                                $configuration->ssh_private_key,
                                $configuration->ssh_password)) {
                                throw new \Exception("Unable to establish connection; "
                                    . "check public/private key credentials");
                            }
                        } else {
                            if (!ssh2_auth_password($this->ssh,
                                $configuration->ssh_username,
                                $configuration->ssh_password)) {
                                throw new \Exception("Unable to establish connection; "
                                    . "check username/password credentials");
                            }
                        }

                        // Determine if DSE INI credentials file should be copied
                        if ($configuration->dse_ini) {
                            if (is_string($configuration->dse_ini) &&
                                !ssh2_scp_send($this->ssh, $configuration->dse_ini,
                                    ".ccm/.dse.ini")) {
                                throw \Exception("Unable to copy DSE INI credentials "
                                    . "file to {$configuration->ssh_host}");
                            }
                        }
                    } else {
                        throw new \Exception("Unable to establish connection to "
                            . $configuration->ssh_host);
                    }
                } else {
                    throw new \Exception("SSH2 is not available");
                }
            }
        }

        // Determine the expected EOL marker should be updated (CCM output)
        if (is_null($this->ssh) &&
            strtoupper(substr(PHP_OS, 0, 3)) === "WIN") {
            $this->eol = PHP_EOL;
        }
    }

    public function __destruct() {
        // Terminate the remote deployment (if applicable)
        if (!is_null($this->ssh)) {
            unset($this->ssh);
        }
    }

    public function __set($property, $value) {
        if (property_exists($this, $property)) {
            $this->$property = $value;
        }
    }

    /**
     * Get the default cluster instance based on the CCM bridge properties
     *
     * @return Cluster Cluster instance
     */
    public function default_cluster() {
        return Cluster::build()
            ->with_dse($this->dse)
            ->with_dse_ini($this->dse_ini)
            ->with_dse_username($this->dse_username)
            ->with_dse_password($this->dse_password)
            ->with_version($this->version)
            ->with_prefix($this->prefix);
    }

    /**
     * Clear the data on the active cluster; as a side effect the cluster is
     * also stopped automicatically if active by CCM
     */
    public function clear_cluster_data() {
        $this->execute_ccm_command("clear");
    }

    /**
     * Get the status for the active cluster
     *
     * @return array Cluster status
     *     [
     *         "decommissioned" => (array[int]) Node number(s) that have been decommissioned
     *         "down"           => (array[int]) Node number(s) that are down
     *         "uninitialized"  => (array[int]) Node number(s) that have not be initialized
     *         "up"             => (array[int]) Node number(s) that are up
     *         "nodes"          => (int) Total number of nodes in the cluster
     *     ]
     */
    public function cluster_status() {
        $decommissioned_nodes = array();
        $down_nodes = array();
        $uninitialized_nodes = array();
        $up_nodes = array();
        $node_count = 0;
        foreach (explode($this->eol, $this->execute_ccm_command("status")) as $status) {
            if ($this->starts_with("node", $status)) {
                // Get the node and status from the node status line
                $key_value = explode(":", $status);
                $node = trim(substr($key_value[0], 4));
                $node_status = trim($key_value[1]);

                // Determine the type of status
                if (strcasecmp("decommisionned", $node_status) == 0 // Handle misspelling in older CCMs
                    || strcasecmp("decommissioned", $node_status) == 0) {
                    $decommissioned_nodes[] = $node;
                } else if (strcasecmp("down", $node_status) == 0) {
                    $down_nodes[] = $node;
                } else if (stripos($node_status, "not initialized") !== false) {
                    $uninitialized_nodes[] = $node;
                } else if (strcasecmp("up", $node_status) == 0) {
                    $up_nodes[] = $node;
                }
                ++$node_count;
            }
        }
        unset($status);

        // Return the status of the cluster
        return array("decommissioned" => $decommissioned_nodes,
            "down" => $down_nodes,
            "uninitialized" => $uninitialized_nodes,
            "up" => $up_nodes,
            "nodes" => $node_count);
    }

    /**
     * Get the comma separated list of IPv4 addresses for nodes in the active
     * cluster
     *
     * @param bool $all (Optional) True if all nodes IPv4 addresses should be
     *                             returned; false if only the `UP` nodes
     *                             (default: true)
     * @return string Comma separated list of IPv4 addresses
     */
    public function contact_points($all = true) {
        if ($all) {
            $ip_prefix = $this->ip_prefix();
            $nodes = $this->cluster_status()["nodes"];
            $contact_points = array();
            foreach (range(1, $nodes) as $node) {
                $contact_points[] = "{$ip_prefix}{$node}";
            }
            return implode(",", $contact_points);
        }

        // Return the liveset (UP) IPv4 addresses
        return $this->execute_ccm_command("liveset");
    }

    /**
     * Create a Cassandra/DSE cluster based on the given cluster configuration
     *
     * @param Cluster $cluster Cluster configuration
     * @return bool True if cluster was created or switched; false otherwise
     * @throws \Exception If no data centers exist in the cluster
     */
    public function create_cluster(Cluster $cluster) {
        // Ensure that there are data centers before creating cluster
        if (count($cluster->data_centers) == 0) {
            throw new \Exception("Unable to Create Cluster: No data centers exist");
        }

        // Determine if the cluster can be switched to or needs to be created
        $clusters = $this->list_clusters();
        if (!$this->switch_cluster($cluster)) {
            // Ensure any active cluster is stopped
            if (!empty($clusters["active"])) {
                $this->kill_cluster();
                $this->clear_cluster_data();
            }

            // Generate the create command for the cluster and execute
            $create_command = array();
            $create_command[] = "create";
            $create_command[] = "-v";
            $create_command[] = $cluster->version;
            if ($cluster->dse) {
                $create_command[] = "--dse";
                if (!$cluster->dse_ini) {
                    $create_command[] = "--dse-username={$cluster->dse_username}";
                    $create_command[] = "--dse-password={$cluster->dse_password}";
                }
            }
            $create_command[] = "-b";
            if ($cluster->ssl) {
                $ssl_path = realpath(__DIR__ . DIRECTORY_SEPARATOR
                    . "ssl") . DIRECTORY_SEPARATOR;
                if (!is_null($this->ssh)) {
                    $files = array_diff(scandir($ssl_path), array("..", "."));
                    var_dump($files);
                    ssh2_exec($this->ssh, "mkdir -p .ccm/ssl");
                    foreach ($files as $file) {
                        $absolute_file = $ssl_path . $file;
                        if (!ssh2_scp_send($this->ssh, $absolute_file, ".ccm/ssl/{$file}")) {
                            throw new \Exception("Unable to Enable SSL: "
                                .   "{$file} could not be copied to {$this->ssh_host}");
                        }
                    }
                    $ssl_path = ".ccm/ssl/";
                }
                $create_command[] = "--ssl={$ssl_path}";
                if ($cluster->client_authentication) {
                    $create_command[] = "--require_client_auth";
                }
            }
            $create_command[] = "{$cluster}";
            call_user_func_array(array($this, "execute_ccm_command"), $create_command);
            $this->apply_base_configuration($cluster->version);

            // Populate the cluster
            $populate_command = array();
            $populate_command[] = "populate";
            $populate_command[] = "-n";
            $populate_command[] = implode(":", $cluster->data_centers);
            $populate_command[] = "-i";
            $populate_command[] = $this->ip_prefix();
            if ($cluster->vnodes) {
                $populate_command[] = "--vnodes";
            }
            call_user_func_array(array($this, "execute_ccm_command"), $populate_command);
            if ($cluster->vnodes) {
                $this->update_cluster_configuration("num_tokens", "1536");
            }

            // Assign DSE workloads (if applicable)
            if ($cluster->dse) {
                $this->set_dse_workload($cluster->workload);
            }
        }

        // Indicate if the cluster was created or switched
        return !($clusters["active"] == "{$cluster}");
    }

    /**
     * Execute a CQL statement on a particular node
     *
     * @param int $node Node to execute CQL statement on
     * @param string $cql CQL statement to execute
     */
    public function execute_cql_on_node($node, $cql) {
        $cqlsh_command = array();
        $cqlsh_command[] = "node{$node}";
        $cqlsh_command[] = "cqlsh";
        $cqlsh_command[] = "-x";
        $cqlsh_command[] = "{$cql};";
        call_user_func_array(array($this, "execute_ccm_command"), $cqlsh_command);
    }

    /**
     * Get the list of IPv4 addresses for the nodes in the active Cassandra/DSE
     * cluster
     *
     * @param bool $all (Optional) True if all nodes IPv4 addresses should be
     *                             returned; false if only the `UP` nodes
     *                             (default: true)
     * @return array Array of IPv4 addresses
     */
    public function ip_addresses($all = true) {
        $ip_addresses = explode(",", $this->contact_points($all));
        sort($ip_addresses);
        return $ip_addresses;
    }

    /**
     * Get the IPv4 prefix (local or remote deployment)
     *
     * @return string IPv4 prefix
     */
    public function ip_prefix() {
        if (is_null($this->ssh)) {
            return "127.0.0.";
        }
        return substr($this->ssh_host, 0, -1);

    }

    /**
     * Check to see if the active cluster is no longer accepting connections
     *
     * NOTE: This method may check the status of the nodes in the cluster
     *       multiple times
     *
     * @return bool True if cluster is no longer accepting connections; false
     *              otherwise
     */
    public function is_cluster_down() {
        $nodes = $this->cluster_status()["nodes"];
        foreach (range(1, $nodes) as $node) {
            if (!$this->is_node_down($node)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check to see if the active cluster is ready to accept connections
     *
     * NOTE: This method may check the status of the nodes in the cluster
     *       multiple times
     *
     * @return bool True if cluster is ready to accept connections; false
     *              otherwise
     */
    public function is_cluster_up() {
        $nodes = $this->cluster_status()["nodes"];
        foreach (range(1, $nodes) as $node) {
            if (!$this->is_node_up($node)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check to see if a node will no longer accept connections
     *
     * NOTE: This method may check the status of the node multiple times
     *
     * @param int $node Node to check `DOWN` status
     * @return bool True if node is no longer accepting connections; false
     *              otherwise
     */
    public function is_node_down($node) {
        foreach (range(1, MAX_RETRIES) as $count) {
            if (!$this->is_node_available($node)) {
                return true;
            } else {
                if ($this->verbose) {
                    $active_cluster = $this->list_clusters()["active"];
                    echo "CCM: Connected to Node {$node} in Cluster {$active_cluster}: "
                        . "Rechecking node down status [{$count}]" . PHP_EOL;
                }
                usleep(NAP);
            }
        }

        // Connection can still be established on node
        return false;
    }

    /**
     * Check to see if a node is ready to accept connections
     *
     * NOTE: This method may check the status of the node multiple times
     *
     * @param int $node Node to check `UP` status
     * @return bool True if node is ready to accept connections; false
     *              otherwise
     */
    public function is_node_up($node) {
        foreach (range(1, MAX_RETRIES) as $count) {
            if ($this->is_node_available($node)) {
                return true;
            } else {
                if ($this->verbose) {
                    $active_cluster = $this->list_clusters()["active"];
                    echo "CCM: Unable to Connect to Node {$node} in Cluster {$active_cluster}: "
                        . "Rechecking node up status [{$count}]" . PHP_EOL;
                }
                usleep(NAP);
            }
        }

        // Connection cannot be established on node
        return false;
    }

    /**
     * Get the list of available Cassandra/DSE clusters
     *
     * @return array List of clusters available to CCM
     *     [
     *         "active" => (string) Active cluster
     *         "list"   => (array[string]) Cluster available (include active
     *                                     cluster)
     *     ]
     */
    public function list_clusters() {
        $active = "";
        $clusters = array();
        foreach (explode($this->eol, $this->execute_ccm_command("list")) as $cluster) {
            // Remove the extra spaces and active cluster indication
            $cluster_name = trim(substr($cluster, 2, strlen($cluster) - 2));

            // Determine if this cluster is the active cluster
            if ($this->is_cluster_active($cluster)) {
                $active = $cluster_name;
            }

            // Add the cluster to the list
            if (!empty($cluster_name)) {
                $clusters[] = $cluster_name;
            }
        }
        return array("active" => $active, "list" => $clusters);
    }

    /**
     * Kill the active cluster (force)
     *
     * @return bool True if cluster is down; false otherwise
     *
     * @see Bridge::stop_cluster()
     */
    public function kill_cluster() {
        return $this->stop_cluster(true);
    }

    /**
     * Kill the given node on the active cluster (force)
     *
     * @param int $node Node to kill
     * @return bool True if node is down; false otherwise
     *
     * @see Bridge::stop_node()
     */
    public function kill_node($node) {
        return $this->stop_node($node, true);
    }

    /**
     * Enable/Disable the binary protocol for a given node on the active
     * cluster
     *
     * @param int $node Node to enable/disable binary protocol
     * @param bool $enable (Optional) True if binary protocol should be enabled;
     *                                false otherwise (default: true)
     */
    public function node_binary($node, $enable = true) {
        $binary_command = array();
        $binary_command[] = "node{$node}";
        $binary_command[] = "nodetool";
        $binary_command[] = ($enable ? "enablebinary" : "disablebinary");
        call_user_func_array(array($this, "execute_ccm_command"), $binary_command);
    }

    /**
     * Enable/Disable gossip for a given node on the active cluster
     *
     * @param int $node Node to enable/disable gossip
     * @param bool $enable (Optional) True if gossip should be enabled; false
     *                                otherwise (default: true)
     */
    public function node_gossip($node, $enable = true) {
        $gossip_command = array();
        $gossip_command[] = "node{$node}";
        $gossip_command[] = "nodetool";
        $gossip_command[] = ($enable ? "enablegossip" : "disablegossip");
        call_user_func_array(array($this, "execute_ccm_command"), $gossip_command);
    }

    /**
     * Enable/Disable trace for a given node on the active cluster
     *
     * @param int $node Node to enable/disable trace
     * @param bool $enable (Optional) True if traceuld be enabled; false
     *                                otherwise (default: true)
     */
    public function node_trace($node, $enable = true) {
        $trace_command = array();
        $trace_command[] = "node{$node}";
        $trace_command[] = "nodetool";
        $trace_command[] = "settraceprobability";
        $trace_command[] = ($enable ? "1" : "0");
        call_user_func_array(array($this, "execute_ccm_command"), $trace_command);
    }

    /**
     * Pause a node on the active cluster
     *
     * @param int $node Node to pause
     */
    public function pause_node($node) {
        $pause_command = array();
        $pause_command[] = "node{$node}";
        $pause_command[] = "pause";
        call_user_func_array(array($this, "execute_ccm_command"), $pause_command);
    }

    /**
     * Remove a cluster
     *
     * @param mixed|null $cluster (Optional) Cluster (or cluster name) to remove
     *                                       (default: active cluster)
     */
    public function remove_cluster($cluster = null) {
        $remove_command = array();
        $remove_command[] = "remove";
        if (is_null($cluster)) {
            $cluster = $this->list_clusters()["active"];
        }
        $remove_command[] = "{$cluster}";
        call_user_func_array(array($this, "execute_ccm_command"), $remove_command);
    }

    /**
     * Remove all available clusters
     *
     * @param bool $all (Optional) True if all clusters should be removed;
     *                             false to remove only CCM::Bridge prefixed
     *                             clusters (default: false)
     */
    public function remove_clusters($all = false) {
        foreach($this->list_clusters()["list"] as $cluster) {
            if ($all || $this->starts_with($this->prefix, $cluster)) {
                $this->remove_cluster($cluster);
            }
        }
    }

    /**
     * Resume a node on the active cluster
     *
     * @param int $node Node to resume
     */
    public function resume_node($node) {
        $resume_command = array();
        $resume_command[] = "node{$node}";
        $resume_command[] = "resume";
        call_user_func_array(array($this, "execute_ccm_command"), $resume_command);
    }

    /**
     * Set the DSE workload on the active cluster or node
     *
     * NOTE: This operation should be performed before starting the
     *       cluster/node; otherwise the cluster/node will be stopped and
     *       restarted
     *
     * @param array[Workload] $workload Workload(s) to apply
     * @param int|null $node (Optional) Node to apply DSE workload on
     *                                  (default: apply DSE workload on cluster)
     * @return bool True if node was restarted; false otherwise
     * @throws \Exception If there are no workloads to apply
     */
    public function set_dse_workload(array $workload, $node = null) {
        if (count($workload) == 0) {
            throw new \Exception("No workloads to assign");
        }

        // Create the DSE workload apply command
        $is_cluster = is_null($node) ? true : false;
        $dse_workload_command = array();
        if (!$is_cluster) {
            $dse_workload_command = "node{$node}";
        }
        $dse_workload_command[] = "setworkload";
        $dse_workload = implode(",", $workload);
        $dse_workload_command[] = $dse_workload;

        // Determine if we are assigning the workload to the cluster or a node
        $was_active = false;
        if ($is_cluster) {
            if (!$this->is_cluster_down()) {
                if ($this->verbose) {
                    echo "Stopping Active Cluster to Set Workload: "
                     . "{$dse_workload} workload" . PHP_EOL;
                }
                $this->stop_cluster();
                $was_active = true;
            }
        } else {
            if (!$this->is_node_down($node)) {
                if ($this->verbose) {
                    echo "Stopping Active Node to Set Workload: "
                        . "{$dse_workload} workload" . PHP_EOL;
                }
                $this-$this->stop_node($node);
                $was_active = true;
            }
        }

        // Execute the CCM command to apply the workloads
        call_user_func_array(array($this, "execute_ccm_command"), $dse_workload_command);

        // Determine if the cluster or node should be restarted
        if ($was_active) {
            if ($this->verbose) {
                echo "Restarting " . ($is_cluster ? "Cluster" : "Node")
                    . " to Apply Workload: {$dse_workload}" . PHP_EOL;
            }
            if ($is_cluster) {
                $this->start_cluster();
            } else {
                $this->start_node($node);
            }
        }

        // Indicate the whether the cluster/node was active when applying workloads
        return $was_active;
    }

    /**
     * Start the active Cassandra/DSE cluster
     *
     * @param array $jvm_arguments (Optional) JVM arguments to apply during
     *                                        cluster start
     *                                        (default: no JVM arguments)
     * @return bool True if cluster is up; false otherwise
     */
    public function start_cluster($jvm_arguments = array()) {
        call_user_func_array(array($this, "execute_ccm_command"),
            $this->generate_start_command($jvm_arguments));
        return $this->is_cluster_up();
    }

    /**
     * Start a node on the active Cassandra/DSE cluster
     *
     * @param int $node Node to start
     * @param array $jvm_arguments (Optional) JVM arguments to apply during
     *                                        cluster start
     *                                        (default: no JVM arguments)
     * @return bool True if node is up; false otherwise
     */
    public function start_node($node, $jvm_arguments = array()) {
        $start_node_command = array();
        $start_node_command[] = "node{$node}";
        $start_node_command = array_merge($start_node_command,
            $this->generate_start_command($jvm_arguments));
        call_user_func_array(array($this, "execute_ccm_command"),
            $start_node_command);
        return $this->is_node_up($node);
    }

    /**
     * Stop the active Cassandra/DSE cluster
     *
     * @param bool $kill (Optional) True if cluster to be forcefully terminated;
     *                              false otherwise (default: false)
     * @return bool True if cluster is down; false otherwise
     */
    public function stop_cluster($kill = false) {
        if ($kill) {
            $this->execute_ccm_command("stop");
        } else {
            $this->execute_ccm_command("stop", "--not-gently");
        }
        return $this->is_cluster_down();
    }

    /**
     * Stop a node on the active Cassandra/DSE cluster
     *
     * @param int $node Node to stop
     * @param bool $kill (Optional) True if node should be forcefully terminated;
     *                              false otherwise (default: false)
     * @return bool True if node is down; false otherwise
     */
    public function stop_node($node, $kill = false) {
        $node_name = "node{$node}";
        if ($kill) {
            $this->execute_ccm_command($node_name, "stop");
        } else {
            $this->execute_ccm_command($node_name, "stop", "--not-gently");
        }
        return $this->is_node_down($node);
    }

    /**
     * Switch to another available cluster
     *
     * @param Cluster $cluster Cluster configuration to switch to
     * @return bool True if switched or is the active/current cluster; false
     *              otherwise
     */
    public function switch_cluster(Cluster $cluster) {
        $clusters = $this->list_clusters();
        if ($clusters["active"] != "{$cluster}") {
            if (in_array("{$cluster}", $clusters)) {
                // Ensure any active cluster is stopped
                if (!empty($clusters["active"])) {
                    $this->kill_cluster();
                    $this->clear_cluster_data();
                    $this->execute_ccm_command("switch", "{$cluster}");
                    return true;
                }
            }
        } else {
            // Cluster requested is already active
            return true;
        }
        return false;
    }

    /**
     * Update the Cassandra/DSE cluster configuration
     *
     * @param mixed $key_or_key_value_pairs Key or key:value pair(s) to update
     * @param mixed|null $value (Optional) Value to apply with key
     *                                     (default: use key/value pair(s))
     * @param bool $is_dse (Optional) True if DSE configuration should be
     *                                updated; false otherwise (default: false)
     * @throws \Exception If value is null when using key:value pair
     */
    public function update_cluster_configuration($key_or_key_value_pairs, $value = null, $is_dse = false) {
        $updateconf_command = array();
        $updateconf_command[] = $is_dse ? "updatedseconf" : "updateconf";
        if (is_array($key_or_key_value_pairs)) {
            $updateconf_command = array_merge($updateconf_command, $key_or_key_value_pairs);
        } else {
            if (is_null($value)) {
                throw new \Exception("Unable to Update Cluster Configuration: Value is null");
            }
            $updateconf_command[] = "{$key_or_key_value_pairs}:{$value}";
        }
        call_user_func_array(array($this, "execute_ccm_command"), $updateconf_command);
    }

    /**
     * Update the DSE cluster configuration
     *
     * @param mixed $key_or_key_value_pairs Key or key:value pair(s) to update
     * @param mixed|null $value (Optional) Value to apply with key
     *                                     (default: use key/value pair(s))
     *
     * @see Bridge::update_cluster_configuration()
     */
    public function update_dse_cluster_configuration($key_or_key_value_pairs, $value = null) {
        $this->update_cluster_configuration($key_or_key_value_pairs, $value, true);
    }

    /**
     * Apply the base/default configuration for Cassandra/DSE cluster
     *
     * @param \Cassandra\Version|\Dse\Version $version Version to apply base
     *                                                 configuration against
     */
    private function apply_base_configuration($version) {
        // Get the Cassandra version
        if ($version instanceof \Dse\Version) {
            $dse_version = $version;
            $version = $version->cassandra_version;
        }

        // Standard configuration elements for all Cassandra versions
        $configuration = array();
        $configuration[] = "--rt=10000";
        $configuration[] = "read_request_timeout_in_ms:10000";
        $configuration[] = "write_request_timeout_in_ms:10000";
        $configuration[] = "request_timeout_in_ms:10000";
        $configuration[] = "phi_convict_threshold:16";
        $configuration[] = "hinted_handoff_enabled:false";
        $configuration[] = "dynamic_snitch_update_interval_in_ms:1000";
        //$configuration[] = "native_transport_max_threads:1";
        $configuration[] = "rpc_min_threads:1";
        $configuration[] = "rpc_max_threads:1";
        $configuration[] = "concurrent_reads:2";
        $configuration[] = "concurrent_writes:2";
        $configuration[] = "concurrent_compactors:1";
        $configuration[] = "compaction_throughput_mb_per_sec:0";
        $configuration[] = "key_cache_size_in_mb:0";
        $configuration[] = "key_cache_save_period:0";
        $configuration[] = "memtable_flush_writers:1";
        $configuration[] = "max_hints_delivery_threads:1";

        // Create Cassandra version specific updates (C* v1.2.x)
        if ($version->compare("2.0.0") < 0) {
            $configuration[] = "reduce_cache_sizes_at:0";
            $configuration[] = "reduce_cache_capacity_to:0";
            $configuration[] = "flush_largest_memtables_at:0";
            $configuration[] = "index_interval:512";
        } else {
            $configuration[] = "cas_contention_timeout_in_ms:10000";
            $configuration[] = "file_cache_size_in_mb:0";
        }

        // Create Cassandra version specific updates (C* < v2.1)
        if ($version->compare("2.1.0") < 0) {
            $configuration[] = "in_memory_compaction_limit_in_mb:1";
        }

        // Create Cassandra version specific updated (C* 2.2+)
        if ($version->compare("2.2.0") >= 0) {
            $configuration[] = "enable_user_defined_functions:true";
        }

        // Create Cassandra version specific updated (C* 3.0+)
        if ($version->compare("3.0.0") >= 0) {
            $configuration[] = "enable_scripted_user_defined_functions:true";
        }

        // Execute the updateconf CCM command
        $this->update_cluster_configuration($configuration);
    }

    /**
     * Execute the CCM command
     *
     * @param array[string] Command arguments to execute
     * @return string Output from the CCM command executed
     */
    private function execute_ccm_command() {
        // Build the CCM command to execute (handle special case on Windows)
        $args = func_get_args();
        foreach ($args as $i => $arg) {
            $args[$i] = escapeshellarg($arg);
        }
        unset($arg);
        $command = sprintf("ccm %s", implode(" ", $args));
        if (is_null($this->ssh) &&
                (strtoupper(substr(PHP_OS, 0, 3)) === "WIN" ||
                strtoupper(substr(PHP_OS, 0, 6)) === "CYGWIN")) {
            $keep_window_context = "";
            if ($args[0] != "\"start\"") {
                $keep_window_context = '/B ';
            }
            $command = 'START "PHP Driver - CCM" ' . $keep_window_context . '/MIN /WAIT ' . $command;
        }

        // Output the command (if verbose)
        if ($this->verbose) {
            echo "CCM: {$command}" . PHP_EOL;
        }

        // Determine if local or remote deployment is being used
        if (is_null($this->ssh)) {
            // Create the process for the command with a 10 minutes timeout (handles source builds
            $process = new \Symfony\Component\Process\Process($command, null, null, null, 600);

            // Execute the command and return the output
            $process->mustRun(function ($type, $buffer) {
                if ($this->verbose) {
                    if (\Symfony\Component\Process\Process::ERR === $type) {
                        echo "CCM: ERROR: {$buffer}";
                    } else {
                        echo "CCM: {$buffer}";
                    }
                }
            });
            return $process->getOutput();
        } else {
            // Execute the command and return the output
            $stream = ssh2_exec($this->ssh, $command);
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
                        echo "CCM: {$output}" . PHP_EOL;
                    }
                }
                return $output;
            }
        }
    }

    /**
     * Generate the start command
     *
     * @param array $jvm_arguments (Optional) JVM arguments to apply with start
     *                                        command generation
     *                                        (default: no JVM arguments)
     * @return array[string] Start command arguments
     */
    private function generate_start_command($jvm_arguments = array()) {
        $start_command[] = "start";
        $start_command[] = "--wait-other-notice";
        $start_command[] = "--wait-for-binary-proto";
        if (is_null($this->ssh) &&
                (strtoupper(substr(PHP_OS, 0, 3)) === "WIN" ||
                strtoupper(substr(PHP_OS, 0, 6)) === "CYGWIN")) {
            $start_command[] = "--quiet-windows";
        }

        // Determine if the JVM argument(s) should be added
        if (count($jvm_arguments) > 0) {
            if (is_array($jvm_arguments)) {
                foreach ($jvm_arguments as $jvm_argument) {
                    $start_command[] = "--jvm_arg={$jvm_argument}";
                }
                unset($jvm_argument);
            } else {
                $start_command[] = "--jvm_arg={$jvm_arguments}";
            }
        }

        return $start_command;
    }

    /**
     * Determine if a cluster is the active cluster
     *
     * @param string $cluster Name of the cluster to check
     * @return bool True if cluster is the active cluster; false otherwise
     */
    private function is_cluster_active($cluster) {
        return $this->starts_with(" *", $cluster);
    }

    /**
     * Determine if a node is available in teh active cluster
     *
     * @param int $node Node to check availability
     * @return bool True if node is available; false otherwise
     */
    private function is_node_available($node) {
        $ip_prefix = $this->ip_prefix();
        $remote_socket = "tcp://{$ip_prefix}{$node}:" . BINARY_PORT;

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
     * Determine if a string starts with another string
     *
     * @param $prefix Prefix/Needle to look for
     * @param $string String/Haystack to check for $prefix
     * @return bool True if $prefix was found in the beginning of $string;
     *              false otherwise
     */
    private function starts_with($prefix, $string) {
        return substr($string, 0, strlen($prefix)) === $prefix;
    }
}

} // namespace CCM