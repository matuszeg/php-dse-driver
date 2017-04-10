@ads
@authentication
@dse-version-5.1
Feature: Kerberos Proxy Authentication

  DSE 5.1 introduced proxy authentication. Proxy authentication allows a client to connect to a node as one user but
  declare that all actions of the client should actually run as a different user (without needing credentials of
  that second user).

  Consult the [DataStax Enterprise Documentation](http://docs.datastax.com/en/dse/5.1/dse-admin/datastax_enterprise/security/secProxy.html)
  for more details.

  Background:
    Given a running DSE cluster with Kerberos authentication enabled
    And the following schema:
      """cql
      CREATE ROLE 'bob@DATASTAX.COM' WITH LOGIN = true;
      CREATE ROLE end_user WITH PASSWORD = 'end_user' and LOGIN = true;
      CREATE ROLE target_user WITH PASSWORD = 'target_user' and LOGIN = true;
      CREATE KEYSPACE simplex WITH REPLICATION = {'class': 'SimpleStrategy', 'replication_factor': 1};
      USE simplex;
      CREATE TABLE gssapi_proxy_auth (f1 int PRIMARY KEY, f2 int);
      INSERT INTO gssapi_proxy_auth (f1, f2) VALUES (1, 2);
      GRANT ALL ON gssapi_proxy_auth TO target_user;
      GRANT PROXY.LOGIN ON ROLE 'target_user' to 'bob@DATASTAX.COM';
      """

  Scenario: Query the authorized table as a proxied user
    Given the following example:
      """php
      // In some Kerberos configurations, hostname resolution must also be enabled
      // for proper communication between the client, DSE, and the Kerberos service.
      // However, if the client uses libuv 0.10.x, hostname resolution is not supported.
      // It is recommended that such deployments use a newer version of libuv.

      $cluster = Dse::cluster()
          ->withGssapiAuthenticator("dse", "bob@DATASTAX.COM", "target_user")
          ->withHostnameResolution(true)
          ->build();
      $session = $cluster->connect("simplex");

      $result = $session->execute("SELECT * from gssapi_proxy_auth");
      foreach ($result as $row) {
        echo "f1: {$row['f1']}  f2: {$row['f2']}" . PHP_EOL;
      }
      """
    And it is executed with the service user Bob's GSSAPI credentials
    When it is executed
    Then its output should contain:
      """
      f1: 1  f2: 2
      """

  Scenario: Query the authorized table as the service user Bob
    Given the following example:
      """php
      try {
          // In some Kerberos configurations, hostname resolution must also be enabled
          // for proper communication between the client, DSE, and the Kerberos service.
          // However, if the client uses libuv 0.10.x, hostname resolution is not supported.
          // It is recommended that such deployments use a newer version of libuv.
          $cluster = Dse::cluster()
              ->withGssapiAuthenticator("dse", "bob@DATASTAX.COM")
              ->withHostnameResolution(true)
              ->build();
          $session = $cluster->connect("simplex");

          $session->execute("SELECT * from gssapi_proxy_auth");
          echo "Request succeeded erroneously!" . PHP_EOL;
      } catch (Dse\Exception\UnauthorizedException $ex) {
          echo $ex->getMessage() . PHP_EOL;
      }
      """
    And it is executed with the service user Bob's GSSAPI credentials
    When it is executed
    Then its output should contain:
      """
      User bob@DATASTAX.COM has no SELECT permission on <table simplex.gssapi_proxy_auth> or any of its parents
      """

  Scenario: Query the authorized table as a proxied user where the service user Bob does not have the PROXY.LOGIN privilege
    Given the following example:
      """php
      try {
          // In some Kerberos configurations, hostname resolution must also be enabled
          // for proper communication between the client, DSE, and the Kerberos service.
          // However, if the client uses libuv 0.10.x, hostname resolution is not supported.
          // It is recommended that such deployments use a newer version of libuv.
          $cluster = Dse::cluster()
              ->withGssapiAuthenticator("dse", "bob@DATASTAX.COM", "end_user")
              ->withHostnameResolution(true)
              ->build();
          $session = $cluster->connect("simplex");
          echo "Connection succeeded erroneously!" . PHP_EOL;
      } catch (Dse\Exception\AuthenticationException $ex) {
          echo $ex->getMessage() . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Received error response 'Failed to login. Please re-try.' (0x02000100)
      """
