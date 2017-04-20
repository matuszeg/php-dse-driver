@migration
@dse-version-5.0
Feature: Migrating from the DataStax Driver for Apache Cassandra to the
  DataStax Enterprise Driver

  Namespace aliasing can be used to ease the transition of existing code to the
  DataStax Enterprise driver.

  Background:
    Given a running DSE cluster

  Scenario: Using namespace aliasing to migrate existing code
    Given the following example:
      """php
      use Dse as Cassandra;

      $cluster = Cassandra::cluster()
        ->build();

      $session = $cluster->connect();

      $row = $session->execute("SELECT release_version FROM system.local")->first();

      echo "Release version: {$row["release_version"]}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Release version:
      """
