@authentication
@dse-version-5.1
Feature: Proxy Execution

  DSE 5.1 introduced proxy execution. Proxy execution allows a client to connect to a node as one user but
  declare that some statements should actually run as a different user (without needing credentials of
  that second user).

  Consult the [DataStax Enterprise Documentation](http://docs.datastax.com/en/dse/5.1/dse-admin/datastax_enterprise/security/secProxy.html)
  for more details.

  Background:
    Given a running DSE cluster with DSE plaintext authentication enabled
    And the following schema:
      """cql
      CREATE ROLE end_user WITH PASSWORD = 'end_user' and LOGIN = true;
      CREATE ROLE target_user WITH PASSWORD = 'target_user' and LOGIN = true;
      CREATE ROLE service_user WITH PASSWORD = 'service_user' and LOGIN = true;
      CREATE KEYSPACE simplex WITH REPLICATION = {'class': 'SimpleStrategy', 'replication_factor': 1};
      USE simplex;
      CREATE TABLE proxy_execute (f1 int PRIMARY KEY, f2 int);
      INSERT INTO proxy_execute (f1, f2) VALUES (1, 2);
      GRANT ALL ON proxy_execute TO target_user;
      GRANT PROXY.EXECUTE ON ROLE 'target_user' to 'service_user';
      """

  Scenario: Query the authorized table as a proxied user
    Given the following example:
      """php
      $cluster = Dse::cluster()
          ->withPlaintextAuthenticator("service_user", "service_user")
          ->build();
      $session = $cluster->connect("simplex");

      $result = $session->execute("SELECT * from proxy_execute", array("execute_as" => "target_user"));
      foreach ($result as $row) {
        echo "f1: {$row['f1']}  f2: {$row['f2']}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      f1: 1  f2: 2
      """

  Scenario: Query the authorized table as the service user
    Given the following example:
      """php
      try {
          $cluster = Dse::cluster()
              ->withPlaintextAuthenticator("service_user", "service_user")
              ->build();
          $session = $cluster->connect("simplex");

          $session->execute("SELECT * from proxy_execute");
          echo "Request succeeded erroneously!" . PHP_EOL;
      } catch (Dse\Exception\UnauthorizedException $ex) {
          echo $ex->getMessage() . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      User service_user has no SELECT permission on <table simplex.proxy_execute> or any of its parents
      """

  Scenario: Query the authorized table as a proxied user where the service user does not have the PROXY.EXECUTE privilege
    Given the following example:
      """php
      try {
          $cluster = Dse::cluster()
              ->withPlaintextAuthenticator("service_user", "service_user")
              ->build();
          $session = $cluster->connect("simplex");
          $session->execute("SELECT * from proxy_execute", array("execute_as" => "end_user"));
          echo "Request succeeded erroneously!" . PHP_EOL;
      } catch (Dse\Exception\UnauthorizedException $ex) {
          echo $ex->getMessage() . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Either 'service_user' does not have permission to execute queries as 'end_user' or that role does not exist. Run 'GRANT PROXY.EXECUTE ON ROLE 'end_user' TO 'service_user' as an administrator if you wish to allow this.
      """
