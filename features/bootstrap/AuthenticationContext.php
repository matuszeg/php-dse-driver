<?php

/**
 * Copyright (c) 2016 DataStax, Inc.
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
        self::$ads = new EmbeddedAds(self::$configuration->verbose);
        self::$ads->start();
        self::$ads->acquire_ticket(EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL,
            self::$ads->cassandra_keytab_file);
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
                "authenticator:com.datastax.bdp.cassandra.auth.DseAuthenticator"
            ),
            "dse" => array(
                "authentication_options.enabled:true",
                "authentication_options.default_scheme:internal",
                "authentication_options.plain_text_without_ssl:allow"
            )
        );

        // Start the DSE cluster
        $this->a_running_cluster_with_nodes(1, $configuration);
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
        $jvm = array();
        if (self::$configuration->version->compare("5.0.0") >= 0) {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.DseAuthenticator";
            $dse[] = "authentication_options.enabled:true";
            $dse[] = "authentication_options.default_scheme:kerberos";
            $dse[] = "authentication_options.scheme_permissions:true";
            $dse[] = "authentication_options.allow_digest_with_kerberos:true";
            $dse[] = "authentication_options.transitional_mode:disabled";
        } else {
            $cassandra[] = "authenticator:com.datastax.bdp.cassandra.auth.KerberosAuthenticator";
        }
        $dse[] = "kerberos_options.service_principal:" . EmbeddedAds::DSE_SERVICE_PRINCIPAL;
        $dse[] = "kerberos_options.http_principal:" . EmbeddedAds::DSE_SERVICE_PRINCIPAL;
        $dse[] = "kerberos_options.keytab:" . self::$ads->dse_keytab_file;
        $dse[] = "kerberos_options.qop:auth";
        $jvm[] = "-Dcassandra.superuser_setup_delay_ms=0";
        $jvm[] = "-Djava.security.krb5.conf=" . self::$ads->configuration_file;

        // Start the DSE cluster
        $this->a_running_cluster_with_nodes(1, array(
            "cassandra" => $cassandra,
            "dse" => $dse,
            "jvm" => $jvm
        ));
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
        $dse[] = "ldap_options.server_host:" . EmbeddedAds::HOST;
        $dse[] = "ldap_options.server_port:" . EmbeddedAds::LDAP_PORT;
        $dse[] = "ldap_options.user_search_base:ou=users,dc=datastax,dc=com";
        $dse[] = "ldap_options.user_search_filter:(uid={0})";
        $jvm[] = "-Dcassandra.superuser_setup_delay_ms=0";

        // Start the DSE cluster
        $this->a_running_cluster_with_nodes(1, array(
            "cassandra" => $cassandra,
            "dse" => $dse,
            "jvm" => $jvm
        ));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: dse, P: @see EmbeddedAds::DSE_SERVICE_PRINCIPAL
     *
     * @Given it is executed with proper GSSAPI credentials
     */
    public function executed_with_proper_gssapi_credentials() {
        self::$ads->acquire_ticket(EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL,
            self::$ads->cassandra_keytab_file);

        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG=" . self::$ads->configuration_file,
            "SERVICE=dse",
            "PRINCIPAL=". EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
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
            "KRB5_CONFIG=" . self::$ads->configuration_file,
            "SERVICE=dse",
            "PRINCIPAL=invalid@". EmbeddedAds::REALM
        )));
    }

    /**
     * Execute the PHP example code with GSSAPI service and principal
     * environment variables; S: invalid, P: @see EmbeddedAds::DSE_SERVICE_PRINCIPAL
     *
     * @Given it is executed with improper GSSAPI service provider, a Dse\Exception\AuthenticationException will occur
     */
    public function executed_with_improper_gssapi_service_provider() {
        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG=" . self::$ads->configuration_file,
            "SERVICE=invalid",
            "PRINCIPAL=". EmbeddedAds::CASSANDRA_SERVICE_PRINCIPAL
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
            self::$ads->dseuser_keytab_file);

        $this->when_executed(array("environment" => array(
            "KRB5_CONFIG=" . self::$ads->configuration_file,
            "SERVICE=dse",
            "PRINCIPAL=". EmbeddedAds::DSEUSER_SERVICE_PRINCIPAL
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
            "USERNAME=cassandra",
            "PASSWORD=cassandra"
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
            "USERNAME=invalid",
            "PASSWORD=credentials"
        )));
    }
}