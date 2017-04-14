<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

use Behat\Behat\Context\Context;
use Behat\Behat\Hook\Scope\AfterFeatureScope;
use Behat\Behat\Hook\Scope\BeforeFeatureScope;

/**
 * Defines the context for the DSE PHP driver authentication features
 */
class AuthenticationContext implements Context {
    // Ensure the common context traits are utilized
    use CommonTraits;

    /**
     * @var EmbeddedAds Embedded ADS for authentication features
     */
    private static $ads;

    /**
     * Initializes context
     *
     * @param DSE\Version $dse_version DSE version to use
     */
    public function __construct($dse_version) {}

    /**
     * Handle feature setup for features that will utilize the ADS
     *
     * @param BeforeFeatureScope $scope The scope before the feature has
     *                                  started
     * @BeforeFeature @ads
     */
    public static function setup_feature_ads(BeforeFeatureScope $scope) {
        self::$ads = new EmbeddedAds(self::$configuration);
        self::$ads->start();
        self::$ads->acquire_ticket(EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL,
            self::$ads->cassandra_keytab_file["client"]);
    }

    /**
     * Handle feature teardown for features that utilize the ADS
     *
     * @param AfterFeatureScope $scope The scope after the feature has
     *                                 completed
     * @AfterFeature @ads
     */
    public static function teardown_feature_ads(AfterFeatureScope $scope) {
        self::$ads->stop();
    }

    /**
     * Create a DSE cluster with a single node that is configured with
     * DSE authentication enabled
     *
     * @Given a running DSE cluster with DSE plaintext authentication enabled
     */
    public function a_running_dse_cluster_with_dse_plaintext_authentication_enabled() {
        // Create and update the configuration elements
        $configuration = array(
            "cassandra" => array(
                "authenticator:com.datastax.bdp.cassandra.auth.DseAuthenticator",
                "authorizer:com.datastax.bdp.cassandra.auth.DseAuthorizer"
            ),
            "dse" => array(
                "authentication_options.enabled:true",
                "authentication_options.default_scheme:internal",
                "authentication_options.scheme_permissions:false",
                "authentication_options.plain_text_without_ssl:allow",
                "authorization_options.enabled:true"
            )
        );

        // Start the DSE cluster
        $this->a_running_cluster_with_nodes("DSE", 1, $configuration);
    }

    /**
     * Create a DSE cluster with a single node that is configured with
     * Kerberos authentication enabled
     *
     * @Given a running DSE cluster with Kerberos authentication enabled
     */
    public function a_running_dse_cluster_with_kerberos_authentication_enabled() {
        // Create and update the configuration elements
        $cassandra = array();
        $dse = array();
        $dse_yaml = array();
        $jvm = array();
        if (self::$configuration->version->compare("5.0.0") >= 0) {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.DseAuthenticator";
            $cassandra[] = "authorizer:com.datastax.bdp.cassandra.auth.DseAuthorizer";
            $dse[] = "authorization_options.enabled:true";
        }
        if (self::$configuration->version->compare("5.0.0") >= 0 &&
            self::$configuration->version->compare("5.1.0") < 0) {
            $dse[] = "authentication_options.enabled:true";
            $dse[] = "authentication_options.default_scheme:kerberos";
            $dse[] = "authentication_options.scheme_permissions:true";
            $dse[] = "authentication_options.allow_digest_with_kerberos:true";
            $dse[] = "authentication_options.transitional_mode:normal";
        } else if (self::$configuration->version->compare("5.1.0") >= 0) {
            $dse_yaml[] = "authentication_options:\n"
                . "  allow_digest_with_kerberos: true\n"
                . "  default_scheme: kerberos\n"
                . "  enabled: true\n"
                . "  other_schemes:\n"
                . "    - internal\n"
                . "  scheme_permissions: true\n"
                . "  transitional_mode: normal";
        } else {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.KerberosAuthenticator";
        }
        $dse[] = "kerberos_options.service_principal:" . EmbeddedAds::DSE_SERVICE_PRINCIPAL;
        $dse[] = "kerberos_options.http_principal:" . EmbeddedAds::DSE_SERVICE_PRINCIPAL;
        $dse[] = "kerberos_options.keytab:" . self::$ads->dse_keytab_file["server"];
        $dse[] = "kerberos_options.qop:auth";
        $jvm[] = "-Dcassandra.superuser_setup_delay_ms=0";
        $jvm[] = "-Djava.security.krb5.conf=" . self::$ads->configuration_file["server"];

        // Start the DSE cluster
        $configuration = array(
            "cassandra" => $cassandra,
            "dse" => $dse,
            "jvm" => $jvm
        );
        if (count($dse_yaml) > 0) {
            $configuration["dse_yaml"] = $dse_yaml;
        }
        $this->a_running_cluster_with_nodes("DSE", 1, $configuration);
    }

    /**
     * Create a DSE cluster with a single node that is configured with
     * LDAP authentication enabled
     *
     * @Given a running DSE cluster with LDAP authentication enabled
     */
    public function a_running_dse_cluster_with_ldap_authentication_enabled() {
        // Create and update the configuration elements
        $cassandra = array();
        $dse = array();
        $jvm = array();
        if (self::$configuration->version->compare("5.0.0") >= 0) {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.DseAuthenticator";
            $dse[] = "authentication_options.enabled:true";
            $dse[] = "authentication_options.default_scheme:ldap";
        } else {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.LdapAuthenticator";
        }
        $dse[] = "ldap_options.search_dn:uid=cassandra,ou=users,dc=datastax,dc=com";
        $dse[] = "ldap_options.search_password:cassandra";
        $dse[] = "ldap_options.server_host:" . self::$ads->host;
        $dse[] = "ldap_options.server_port:" . EmbeddedAds::LDAP_PORT;
        $dse[] = "ldap_options.user_search_base:ou=users,dc=datastax,dc=com";
        $dse[] = "ldap_options.user_search_filter:(uid={0})";
        $jvm[] = "-Dcassandra.superuser_setup_delay_ms=0";

        // Start the DSE cluster
        $this->a_running_cluster_with_nodes("DSE", 1, array(
            "cassandra" => $cassandra,
            "dse" => $dse,
            "jvm" => $jvm
        ));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: dse, P: @see EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
     *
     * @Given it is executed with proper GSSAPI credentials
     */
    public function executed_with_proper_gssapi_credentials() {
        self::$ads->acquire_ticket(EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL,
            self::$ads->cassandra_keytab_file["client"]);

        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG" => self::$ads->configuration_file["client"],
            "SERVICE" => "dse",
            "PRINCIPAL" => EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
        )));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: dse, P: @see EmbeddedAds::BOB_SERVICE_PRINCIPAL
     *
     * @Given it is executed with the service user Bob's GSSAPI credentials
     */
    public function executed_with_bob_gssapi_credentials() {
        self::$ads->acquire_ticket(EmbeddedAds::BOB_PRINCIPAL,
            self::$ads->bob_keytab_file["client"]);

        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG" => self::$ads->configuration_file["client"],
            "SERVICE" => "dse",
            "PRINCIPAL" => EmbeddedAds::BOB_PRINCIPAL
        )));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: dse, P: invalid (@see EmbeddedAds::REALM)
     *
     * @Given it is executed with improper GSSAPI principal, a Dse\Exception\AuthenticationException will occur
     */
    public function executed_with_improper_gssapi_principal() {
        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG" => self::$ads->configuration_file["client"],
            "SERVICE" => "dse",
            "PRINCIPAL" => "invalid@". EmbeddedAds::REALM
        )));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: invalid, P: @see EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
     *
     * @Given it is executed with improper GSSAPI service provider, a Dse\Exception\AuthenticationException will occur
     */
    public function executed_with_improper_gssapi_service_provider() {
        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG" => self::$ads->configuration_file["client"],
            "SERVICE" => "invalid",
            "PRINCIPAL" => EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
        )));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: dse, P: EmbeddedAds::DSEUSER_SERVICE_PRINCIPAL
     *
     * @Given it is executed with unauthorized GSSAPI principal, a Dse\Exception\AuthenticationException will occur
     */
    public function executed_with_unauthorized_gssapi_principal() {
        // Ensure the dseuser principal ticket is available
        self::$ads->acquire_ticket(EmbeddedAds::DSEUSER_SERVICE_PRINCIPAL,
            self::$ads->dseuser_keytab_file["client"]);

        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG" => self::$ads->configuration_file["client"],
            "SERVICE" => "dse",
            "PRINCIPAL" => EmbeddedAds::DSEUSER_SERVICE_PRINCIPAL
        )));
    }

    /**
     * Execute the PHP example code with username and password environment
     * variables; U: cassandra, P: cassandra
     *
     * @Given it is executed with proper credentials
     */
    public function executed_with_proper_credentials() {
        $this->when_executed(array("environment" => array(
            "USERNAME" => "cassandra",
            "PASSWORD" => "cassandra"
        )));
    }

    /**
     * Execute the PHP example code with username and password environment
     * variables; U: invalid, P: credentials
     *
     * @Given it is executed with improper credentials, a Dse\Exception\AuthenticationException will occur
     */
    public function executed_with_improper_credentials() {
        $this->when_executed(array("environment" => array(
            "USERNAME" => "invalid",
            "PASSWORD" => "credentials"
        )));
    }
}
