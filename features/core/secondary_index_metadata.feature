Feature: Secondary Index Metadata

  PHP Driver exposes the Cassandra Schema Metadata for secondary indexes.

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
      CREATE INDEX IF NOT EXISTS name_index ON users(name);
      """

  Scenario: Getting a index metadata
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");
      $schema = $session->schema();
      $index = $schema->keyspace("simplex")->table("users")->index("name_index");

      echo "Name: {$index->name()}" . PHP_EOL;
      echo "Kind: {$index->kind()}" . PHP_EOL;
      echo "Target: {$index->target()}" . PHP_EOL;
      echo "IsCustom: " . ($index->isCustom() ? "true" : "false") . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Name: name_index
      Kind: composites
      Target: name
      IsCustom: false
      """

  @cassandra-version-3.0
  Scenario: Getting a index metadata with options
    Given the following example:
      """php
      $cluster  = Dse::cluster()->build();
      $session  = $cluster->connect("simplex");

      $schema   = $session->schema();

      $index = $schema->keyspace("simplex")->table("users")->index("name_index");

      echo "Name: {$index->name()}" . PHP_EOL;
      echo "Kind: {$index->kind()}" . PHP_EOL;
      echo "Target: {$index->target()}" . PHP_EOL;
      echo "IsCustom: " . ($index->isCustom() ? "true" : "false") . PHP_EOL;
      echo "Options:" . PHP_EOL;
      foreach ($index->options() as $key => $value) {
          echo "  {$key} => {$value}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Name: name_index
      Kind: composites
      Target: name
      IsCustom: false
      Options:
        target => name
      """

  @cassandra-version-3.0.4
  @cassandra-version-3.4
  Scenario: Getting a custom index metadata
    Given the following schema:
      """
      CREATE CUSTOM INDEX name_custom_index ON simplex.users (name)
        USING
          'org.apache.cassandra.index.internal.composites.ClusteringColumnIndex';
      """
    And the following example:
      """php
      $cluster  = Dse::cluster()->build();
      $session  = $cluster->connect("simplex");

      $schema   = $session->schema();

      $index = $schema->keyspace("simplex")->table("users")->index("name_custom_index");

      echo "Name: {$index->name()}" . PHP_EOL;
      echo "Kind: {$index->kind()}" . PHP_EOL;
      echo "Target: {$index->target()}" . PHP_EOL;
      echo "IsCustom: " . ($index->isCustom() ? "true" : "false") . PHP_EOL;
      echo "ClassName: {$index->className()}" . PHP_EOL;
      echo "Options:" . PHP_EOL;
      foreach ($index->options() as $key => $value) {
          echo "  {$key} => {$value}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Name: name_custom_index
      Kind: custom
      Target: name
      IsCustom: true
      ClassName: org.apache.cassandra.index.internal.composites.ClusteringColumnIndex
      Options:
        class_name => org.apache.cassandra.index.internal.composites.ClusteringColumnIndex
        target => name
      """
