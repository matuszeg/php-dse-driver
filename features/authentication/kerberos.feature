@ads
@authentication
Feature: Kerberos Authentication

  DSE 5.0 introduced a DSE Unified Authenticator. The DSE Authenticator can be
  used for Kerberos authentication by enabling the GSSAPI Authenticator. DSE
  versions earlier than 5.0 are configured similarly but use Kerberos
  Authenticator on the DSE cluster.

  Background:
    Given a running DSE cluster with Kerberos authentication enabled
    And the following example:
    """php
    // Get the service and principal from environment variables
    $service = $_SERVER["SERVICE"];
    $principal = $_SERVER["PRINCIPAL"];

    // Attempt to create the session using GSSAPI authentication
    try {
        $cluster = Dse::cluster()
            ->withGssapiAuthenticator($service, $principal)
            ->build();
        $session = $cluster->connect();

        // Indicate the connection was successful
        echo "Login successful" . PHP_EOL;
    } catch (Dse\Exception\AuthenticationException $der) {
        // Indicate the connection was unsuccessful
        echo $der->getMessage() . PHP_EOL;
    }
    """

    Scenario: Authenticating with proper GSSAPI credentials
      And it is executed with proper GSSAPI credentials
      Then its output should contain:
      """
      Login successful
      """

    Scenario: Authenticating with improper GSSAPI service provider
      But it is executed with improper GSSAPI service provider, a Dse\Exception\AuthenticationException will occur
      Then its output should contain pattern:
      """
      Server .* not found in Kerberos database
      """

    Scenario: Authenticating with improper GSSAPI principal
      But it is executed with improper GSSAPI principal, a Dse\Exception\AuthenticationException will occur
      Then its output should contain:
      """
      Can't find client principal invalid@DATASTAX.COM in cache collection
      """

    Scenario: Authenticating with unauthorized GSSAPI principal
      But it is executed with unauthorized GSSAPI principal, a Dse\Exception\AuthenticationException will occur
      Then its output should contain pattern:
      """
      dseuser@DATASTAX.COM is not permitted to log in|User dseuser@DATASTAX.COM doesn't exist - create it with CREATE USER query first
      """
