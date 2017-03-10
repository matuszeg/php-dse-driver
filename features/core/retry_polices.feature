Feature: Retry Policies

  PHP Driver supports consistency levels when executing statements.

  Background:
    Given a running Cassandra cluster
    Given the following logger settings:
      """ini
      dse.log=feature-retry-policy.log
      dse.log_level=INFO
      """
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

  Scenario: Retry policy can be set when constructing the cluster
    Given tracing is enabled
    And the following example:
      """php
      $retry_policy = new Dse\RetryPolicy\DowngradingConsistency();
      $cluster     = Dse::cluster()
                       ->withRetryPolicy(new Dse\RetryPolicy\Logging($retry_policy))
                       ->build();
      $session     = $cluster->connect("simplex");
      $statement   = $session->prepare("INSERT INTO playlists (id, song_id, artist, title, album)
                                       VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)");
      $arguments   = array(new Dse\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
                          'Joséphine Baker',
                          'La Petite Tonkinoise',
                          'Bye Bye Blackbird'
                          );
      $options     = array(
                          'consistency' => Dse::CONSISTENCY_QUORUM,
                          'arguments' => $arguments
                          );
      $session->execute($statement, $options);

      $statement = new Dse\SimpleStatement("SELECT * FROM simplex.playlists");
      $result    = $session->execute($statement);

      foreach ($result as $row) {
        echo "{$row['artist']}: {$row['title']} / {$row['album']}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird
      """
    Then a log file "feature-retry-policy.log" should exist
    And the log file "feature-retry-policy.log" should contain "Retrying on unavailable error at consistency ONE"


  Scenario: Retry policy can be set when executing a statement
    Given tracing is enabled
    And the following example:
      """php
      $retry_policy = new Dse\RetryPolicy\DowngradingConsistency();
      $cluster     = Dse::cluster()->build();
      $session     = $cluster->connect("simplex");
      $statement   = $session->prepare("INSERT INTO playlists (id, song_id, artist, title, album)
                                        VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)");
      $arguments   = array(new Dse\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
                          'Joséphine Baker',
                          'La Petite Tonkinoise',
                          'Bye Bye Blackbird'
                          );
      $options     = array(
                          'consistency' => Dse::CONSISTENCY_QUORUM,
                          'arguments' => $arguments,
                          'retry_policy' => new Dse\RetryPolicy\Logging($retry_policy)
                          );
      $session->execute($statement, $options);

      $statement = new Dse\SimpleStatement("SELECT * FROM simplex.playlists");
      $result    = $session->execute($statement);

      foreach ($result as $row) {
          echo "{$row['artist']}: {$row['title']} / {$row['album']}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird
      """
    Then a log file "feature-retry-policy.log" should exist
    And the log file "feature-retry-policy.log" should contain "Retrying on unavailable error at consistency ONE"


  Scenario: Retry policy can be set when executing a batch
    Given tracing is enabled
    And the following example:
      """php
      $retry_policy = new Dse\RetryPolicy\DowngradingConsistency();
      $cluster   = Dse::cluster()->build();
      $session   = $cluster->connect("simplex");
      $prepared  = $session->prepare(
                     "INSERT INTO playlists (id, song_id, artist, title, album) " .
                     "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)"
                   );
      $simple    = new Dse\SimpleStatement(
                     "INSERT INTO playlists (id, song_id, artist, title, album) " .
                     "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)"
                   );
      $batch     = new Dse\BatchStatement(Dse::BATCH_UNLOGGED);

      $options    = array(
                         'consistency' => Dse::CONSISTENCY_QUORUM,
                         'retry_policy' => new Dse\RetryPolicy\Logging($retry_policy)
                    );

      $batch->add($prepared, array(
          'song_id' => new Dse\Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
          'title'   => 'La Petite Tonkinoise',
          'album'   => 'Bye Bye Blackbird',
          'artist'  => 'Joséphine Baker'
      ));

      $batch->add($simple, array(
          new Dse\Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'),
          'Willi Ostermann', 'Die Mösch', 'In Gold',
      ));

      $batch->add($prepared, array(
          new Dse\Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'),
          'Mick Jager', 'Memo From Turner', 'Performance'
      ));

      $session->execute($batch, $options);

      $statement = new Dse\SimpleStatement("SELECT * FROM simplex.playlists");
      $result    = $session->execute($statement);

      foreach ($result as $row) {
          echo "{$row['artist']}: {$row['title']} / {$row['album']}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird
      """
    And its output should contain:
      """
      Willi Ostermann: Die Mösch / In Gold
      """
    And its output should contain:
      """
      Mick Jager: Memo From Turner / Performance
      """
    Then a log file "feature-retry-policy.log" should exist
    And the log file "feature-retry-policy.log" should contain "Retrying on unavailable error at consistency ONE"
