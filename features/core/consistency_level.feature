Feature: Consistency Level

  PHP Driver supports consistency levels when executing statements.

  Background:
    Given a running Cassandra cluster with 3 nodes
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 3
      };
      USE simplex;
      CREATE TABLE playlists (
        id uuid,
        title text,
        album text,
        artist text,
        song_id uuid,
        PRIMARY KEY (id, title, album, artist)
      );
      """

  Scenario: Consistency levels are specified via an ExecutionOptions object
    Given tracing is enabled
    And the following example:
      """php
      $cluster     = Dse::cluster()->build();
      $session     = $cluster->connect("simplex");

      $insertQuery = "INSERT INTO playlists (id, song_id, artist, title, album) " .
                     "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, " .
                         new Dse\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50') . ", " .
                         "'Joséphine Baker', " .
                         "'La Petite Tonkinoise', " .
                         "'Bye Bye Blackbird')";

      // ExecutionOptions is deprecated, but still legal. Disable error reporting for it.
      error_reporting(E_ALL ^ E_DEPRECATED);

      $options = new Dse\ExecutionOptions(array('consistency' => Dse::CONSISTENCY_ALL));
      $session->execute($insertQuery, $options);

      // Restore error-reporting to normal.
      error_reporting(E_ALL);

      // Below uses the system_traces.events table to verify consistency ALL is met
      $result    = $session->execute("SELECT source from system_traces.events", $options);
      $sources   = array();
      foreach ($result as $row) {
          array_push($sources, (string) $row['source']);
      }
      $sources = array_unique($sources);

      foreach ($sources as $source) {
          echo str_replace($_SERVER["IP_PREFIX"], "", $source) . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain disregarding order:
      """
      1
      2
      3
      """
  Scenario: Consistency levels are specified via an array of execution options
    Given tracing is enabled
    And the following example:
      """php
      $cluster     = Dse::cluster()->build();
      $session     = $cluster->connect("simplex");

      $insertQuery = "INSERT INTO playlists (id, song_id, artist, title, album) " .
                     "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, " .
                         new Dse\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50') . ", " .
                         "'Joséphine Baker', " .
                         "'La Petite Tonkinoise', " .
                         "'Bye Bye Blackbird')";
      $options     = array('consistency' => Dse::CONSISTENCY_ALL);
      $session->execute($insertQuery, $options);

      // Below uses the system_traces.events table to verify consistency ALL is met
      $result    = $session->execute("SELECT source from system_traces.events", $options);
      $sources   = array();
      foreach ($result as $row) {
          array_push($sources, (string) $row['source']);
      }
      $sources = array_unique($sources);

      foreach ($sources as $source) {
          echo str_replace($_SERVER["IP_PREFIX"], "", $source) . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain disregarding order:
      """
      1
      2
      3
      """
