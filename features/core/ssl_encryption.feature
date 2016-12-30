@SSL
Feature: SSL encryption

  PHP Driver supports SSL encryption.

  Scenario: Connecting without SSL encryption
    Given a running Cassandra cluster with SSL encryption
    And the following example:
      """php
      $cluster = Dse::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->build();

      try {
          $session = $cluster->connect();
      } catch (Dse\Exception\RuntimeException $e) {
          echo "Connection failure" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Connection failure
      """

  Scenario: Connecting with basic SSL encryption
    Given a running Cassandra cluster with SSL encryption
    And the following example:
      """php
      $ssl     = Dse::ssl()
                     ->withVerifyFlags(Dse::VERIFY_NONE)
                     ->build();
      $cluster = Dse::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->withSSL($ssl)
                     ->build();

      try {
          $session = $cluster->connect();
          echo "Connection success" . PHP_EOL;
      } catch (Dse\Exception\RuntimeException $e) {
          echo "Connection failure" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Connection success
      """

  Scenario: Connecting with certificate verification
    Given a running Cassandra cluster with SSL encryption
    And the following example:
      """php
      $ssl     = Dse::ssl()
                     ->withVerifyFlags(Dse::VERIFY_PEER_CERT)
                     ->withTrustedCerts($_SERVER['SERVER_CERT'])
                     ->build();
      $cluster = Dse::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->withSSL($ssl)
                     ->build();

      try {
          $session = $cluster->connect();
          echo "Connection success" . PHP_EOL;
      } catch (Dse\Exception\RuntimeException $e) {
          echo "Connection failure" . PHP_EOL;
      }
      """
    When it is executed with proper SSL credentials
    Then its output should contain:
      """
      Connection success
      """

  Scenario: Connecting with client certificate verification
    Given a running Cassandra cluster with SSL encryption and client authentication
    And the following example:
      """php
      $ssl     = Dse::ssl()
                     ->withVerifyFlags(Dse::VERIFY_PEER_CERT)
                     ->withTrustedCerts($_SERVER['SERVER_CERT'])
                     ->withClientCert($_SERVER['CLIENT_CERT'])
                     ->withPrivateKey($_SERVER['PRIVATE_KEY'], $_SERVER['PASSPHRASE'])
                     ->build();
      $cluster = Dse::cluster()
                     ->withContactPoints('127.0.0.1')
                     ->withSSL($ssl)
                     ->build();

      try {
          $session = $cluster->connect();
          echo "Connection success" . PHP_EOL;
      } catch (Dse\Exception\RuntimeException $e) {
          echo "Connection failure" . PHP_EOL;
      }
      """
    When it is executed with proper SSL credentials
    Then its output should contain:
      """
      Connection success
      """
