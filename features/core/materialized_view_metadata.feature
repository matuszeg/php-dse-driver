@cassandra-version-3.0
Feature: Materialized View Metadata

  PHP Driver exposes the Cassandra Schema Metadata for materialized views.

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      } AND DURABLE_WRITES = false;
      USE simplex;
      CREATE TABLE users (id int PRIMARY KEY, name text);
      CREATE MATERIALIZED VIEW IF NOT EXISTS users_view AS SELECT name FROM users WHERE name IS NOT NULL PRIMARY KEY(name, id);
      """

  Scenario: Getting a materialized view
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");
      $schema = $session->schema();
      $view = $schema->keyspace("simplex")->materializedView("users_view");

      echo "Name => {$view->name()}" . PHP_EOL;
      echo "Base Table => {$view->baseTable()->name()}" . PHP_EOL;
      echo "Default Time To Live => {$view->option("default_time_to_live")}" . PHP_EOL;
      echo "Compression:" . PHP_EOL;
      foreach ($view->option("compression") as $key => $value) {
          echo "  {$key} => {$value}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Name => users_view
      Base Table => users
      Default Time To Live => 0
      Compression:
        chunk_length_in_kb => 64
        class => org.apache.cassandra.io.compress.LZ4Compressor
      """

