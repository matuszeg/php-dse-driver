<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

namespace {
/**
 * This is the base version class for representing application version numbers.
 */
class Version {
    /**
     * @var int Major portion of version number
     */
    private $major;
    /**
     * @var int Minor portion of version number
     */
    private $minor;
    /**
     * @var int Patch portion of version number
     */
    private $patch;
    /**
     * @var mixed Extra portion of version number
     */
    private $extra;

    /**
     * Constructor
     *
     * @param string $version String version representation
     * @throws Exception Invalid version
     */
    public function __construct($version) {
        $this->from_string($version);
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            return $this->$property;
        }
    }

    public function __toString() {
        $version = $this->major . "." . $this->minor;
        if (isset($this->patch)) {
            $version .= "." . $this->patch;
        }
        if (isset($this->extra)) {
            $version .= "-" . $this->extra;
        }
        return $version;
    }

    /**
     * Compare two versions
     *
     * @param mixed $rhs Right-hand side version to compare
     * @return int -1 if LHS < RHS, 1 if LHS > RHS, and 0 if equal
     */
    public function compare($rhs) {
        $version = $rhs;
        if (is_string($rhs)) {
            $version = new Version($rhs);
        }
        return version_compare($this, $version);
    }

    /**
     * Compare two version to see if they are equal
     *
     * @param mixed $rhs Right-hand side version to compare
     * @return bool True if versions are equal; false otherwise
     */
    public function equal($rhs) {
        $version = $rhs;
        if (is_string($rhs)) {
            $version = new Version($rhs);
        }
        return $this->compare($version) == 0;
    }

    /**
     * Convert the version string from Cassandra to the class properties
     *
     * @param string $version Version string to parse
     * @throws Exception Invalid version
     */
    protected function from_string($version) {
        // Validate and parse the version number
        if (preg_match("@(\d+)\.(\d+)\.(\d+)[\.\-](.+)@", $version, $parsed_version)) {
            $this->major =  $parsed_version[1];
            $this->minor = $parsed_version[2];
            $this->patch =  $parsed_version[3];
            $this->extra =  $parsed_version[4];
        } else if (preg_match("@(\d+)\.(\d+)\.(\d+)@", $version, $parsed_version)) {
            $this->major = $parsed_version[1];
            $this->minor = $parsed_version[2];
            $this->patch = $parsed_version[3];
        } else if (preg_match("@(\d+)\.(\d+)@", $version, $parsed_version)) {
            $this->major = $parsed_version[1];
            $this->minor = $parsed_version[2];
        } else if (preg_match("@(\d+)\.(\d+)[\.\-](.++)@", $version, $parsed_version)) {
            $this->major =  $parsed_version[1];
            $this->minor = $parsed_version[2];
            $this->extra =  $parsed_version[3];
        } else {
            throw new Exception("Invalid Version Number {$version}: "
                . "Pattern must follow <major>.<minor>.<patch>(.|-)<extra>");
        }
    }
}
}

namespace Cassandra {
/**
 * This class is for representing Cassandra versions.
 *
 * @package Cassandra
 */
class Version extends \Version {
    /**
     * CQL string to retrieve Cassandra version
     */
    const CQL_SELECT_CASSANDRA_RELEASE_VERSION =
        "SELECT release_version FROM system.local";

    /**
     * Constructor
     *
     * @param string $version String version representation
     * @throws Exception Invalid version
     */
    public function __construct($version) {
        parent::__construct($version);
    }

    /**
     * Get the Cassandra\Version from the server
     *
     * @param Cassandra\Session $session Session object to utilize when
     *                                   gathering the Cassandra version number
     * @return Version Cassandra version from the server
     */
    public static function from_server(Cassandra\Session $session) {
        $result = $session->execute(new Cassandra\SimpleStatement(
            self::CQL_SELECT_CASSANDRA_RELEASE_VERSION));
        return new Version($result[0]["release_version"]);
    }
}
}

namespace Dse {
/**
 * This class is for representing DSE versions as it will encapsulate the
 * Cassandra version.
 *
 * @package Dse
 */
class Version extends \Version {
    /**
     * @var Cassandra\Version Cassandra version used by DSE version
     */
    private $cassandra_version;

    /**
     * CQL string to retrieve DSE version
     */
    const CQL_SELECT_DSE_RELEASE_VERSION =
        "SELECT dse_version FROM system.local";

    /**
     * Constructor
     *
     * @param string $version String version representation
     * @throws Exception Invalid version
     */
    public function __construct($version) {
        parent::__construct($version);
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            if ($property == "cassandra_version") {
                if (isset($this->$property)) {
                    return $this->$property;
                } else {
                    $cassandra_version = $this->cassandra_version();
                    return $cassandra_version;
                }
            }
        }
    }

    /**
     * Get the Cassandra\Version from the server
     *
     * @param Dse\Session $session Session object to utilize when
     *                                   gathering the Cassandra version number
     * @return Version DSE version from the server
     */
    public static function from_server(Dse\Session $session) {
        $result = $session->execute(new Dse\SimpleStatement(
            self::CQL_SELECT_DSE_RELEASE_VERSION));
        $version = new Version($result[0]["dse_version"]);
        $version->cassandra_version = new \Cassandra\Version($session);
        return $version;
    }

    /**
     * Get the Cassandra version used in the DSE version from a 1:1 mapping
     *
     * @return \Cassandra\Version Cassandra version used by DSE version
     * @throws Exception Invalid DSE version
     */
    private function cassandra_version() {
        // Map the DSE version to the appropriate Cassandra version
        if ($this->equal("4.5.0") || $this->equal("4.5.1")) {
            return new \Cassandra\Version("2.0.8-39");
        } else if ($this->equal("4.5.2")) {
            return new \Cassandra\Version("2.0.10-71");
        } else if ($this->equal("4.5.3")) {
            return new \Cassandra\Version("2.0.11-82");
        } else if ($this->equal("4.5.4")) {
            return new \Cassandra\Version("2.0.11-92");
        } else if ($this->equal("4.5.5")) {
            return new \Cassandra\Version("2.0.12-156");
        } else if ($this->equal("4.5.6")) {
            return new \Cassandra\Version("2.0.12-200");
        } else if ($this->equal("4.5.7")) {
            return new \Cassandra\Version("2.0.12-201");
        } else if ($this->equal("4.5.8")) {
            return new \Cassandra\Version("2.0.14-352");
        } else if ($this->equal("4.5.9")) {
            return new \Cassandra\Version("2.0.16-762");
        } else if ($this->equal("4.6.0")) {
            return new \Cassandra\Version("2.0.11-83");
        } else if ($this->equal("4.6.1")) {
            return new \Cassandra\Version("2.0.12-200");
        } else if ($this->equal("4.6.2")) {
            return new \Cassandra\Version("2.0.12-274");
        } else if ($this->equal("4.6.3")) {
            return new \Cassandra\Version("2.0.12-275");
        } else if ($this->equal("4.6.4")) {
            return new \Cassandra\Version("2.0.14-348");
        } else if ($this->equal("4.6.5")) {
            return new \Cassandra\Version("2.0.14-352");
        } else if ($this->equal("4.6.6")) {
            return new \Cassandra\Version("2.0.14-425");
        } else if ($this->equal("4.6.7")) {
            return new \Cassandra\Version("2.0.14-459");
        } else if ($this->equal("4.6.8") || $this->equal("4.6.9")) {
            return new \Cassandra\Version("2.0.16-678");
        } else if ($this->equal("4.6.10")) {
            return new \Cassandra\Version("2.0.16-762");
        } else if ($this->equal("4.6.11")) {
            return new \Cassandra\Version("2.0.17-858");
        } else if ($this->equal("4.6.12")) {
            return new \Cassandra\Version("2.0.17-903");
        } else if ($this->equal("4.6.13")) {
            return new \Cassandra\Version("2.0.17-1420");
        } else if ($this->equal("4.7.0")) {
            return new \Cassandra\Version("2.1.5-469");
        } else if ($this->equal("4.7.1") || $this->equal("4.7.2")) {
            return new \Cassandra\Version("2.1.8-621");
        } else if ($this->equal("4.7.3")) {
            return new \Cassandra\Version("2.1.8-689");
        } else if ($this->equal("4.7.4")) {
            return new \Cassandra\Version("2.1.11-872");
        } else if ($this->equal("4.7.5")) {
            return new \Cassandra\Version("2.1.11-908");
        } else if ($this->equal("4.7.6")) {
            return new \Cassandra\Version("2.1.11-969");
        } else if ($this->equal("4.7.7")) {
            return new \Cassandra\Version("2.1.12-1049");
        } else if ($this->equal("4.7.8")) {
            return new \Cassandra\Version("2.1.13-1218");
        } else if ($this->equal("4.7.9")) {
            return new \Cassandra\Version("2.1.15-1416");
        } else if ($this->equal("4.8.0")) {
            return new \Cassandra\Version("2.1.9-791");
        } else if ($this->equal("4.8.1")) {
            return new \Cassandra\Version("2.1.11-872");
        } else if ($this->equal("4.8.2")) {
            return new \Cassandra\Version("2.1.11-908");
        } else if ($this->equal("4.8.3")) {
            return new \Cassandra\Version("2.1.11-969");
        } else if ($this->equal("4.8.4")) {
            return new \Cassandra\Version("2.1.12-1046");
        } else if ($this->equal("4.8.5")) {
            return new \Cassandra\Version("2.1.13-1131");
        } else if ($this->equal("4.8.6")) {
            return new \Cassandra\Version("2.1.13-1218");
        } else if ($this->equal("4.8.7")) {
            return new \Cassandra\Version("2.1.14-1272");
        } else if ($this->equal("4.8.8")) {
            return new \Cassandra\Version("2.1.14-1346");
        } else if ($this->equal("4.8.9")) {
            return new \Cassandra\Version("2.1.15-1403");
        } else if ($this->equal("4.8.10")) {
            return new \Cassandra\Version("2.1.15-1423");
        } else if (($compare = $this->compare("4.8.11")) >= 0
            && $this->compare("5.0.0") < 0) {
            if ($compare > 0) {
                fwrite(STDERR, "Cassandra Version is not Defined: "
                    . "Add Cassandra version for DSE v{$this}" . PHP_EOL);
            }
            return new \Cassandra\Version("2.1.17.1428");
        } else if ($this->equal("5.0.0")) {
            return new \Cassandra\Version("3.0.7-1158");
        } else if ($this->equal("5.0.1")) {
            return new \Cassandra\Version("3.0.7-1159");
        } else if ($this->equal("5.0.2")) {
            return new \Cassandra\Version("3.0.8-1293");
        } else if ($this->equal("5.0.3")) {
            return new \Cassandra\Version("3.0.9-1346");
        } else if (($compare = $this->compare("5.0.4")) >= 0) {
            if ($compare > 0) {
                fwrite(STDERR, "Cassandra Version is not Defined: "
                    . "Add Cassandra version for DSE v{$this}" . PHP_EOL);
            }
            return new \Cassandra\Version("3.0.10.1443");
        }

        // DSE version does not correspond to a valid Cassandra version
        throw new Exception("Invalid DSE Version {$this}: Could not determine Cassandra version");
    }
}
}