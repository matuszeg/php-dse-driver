@ads
@authentication
Feature: LDAP Authentication

  DSE 5.0 introduced a DSE Unified Authenticator. The DSE Authenticator can be
  used for LDAP authentication by configuring the cluster to use a given
  username/password for authentication to the DSE cluster. DSE versions earlier
  than 5.0 are configured similarly but use LDAP Authenticator on the DSE
  cluster.

  Background:
    Given a running DSE cluster with LDAP authentication enabled
    And the following example:
    """php
    // Get the username and password from environment variables
    $username = $_SERVER["USERNAME"];
    $password = $_SERVER["PASSWORD"];

    // Attempt to create the session using plaintext authentication
    try {
        $cluster = Dse::cluster()
            ->withPlaintextAuthenticator($username, $password)
            ->build();
        $session = $cluster->connect();

        // Indicate the connection was successful
        echo "Login successful" . PHP_EOL;
    } catch (Dse\Exception\AuthenticationException $der) {
        // Indicate the connection was unsuccessful
        echo "Login failed" . PHP_EOL;
    }
    """

    Scenario: Authenticating with proper LDAP credentials
      And it is executed with proper credentials
      Then its output should contain:
      """
      Login successful
      """

    Scenario: Authenticating with improper LDAP credentials
      But it is executed with improper credentials, a Dse\Exception\AuthenticationException will occur
      Then its output should contain:
      """
      Login failed
      """
