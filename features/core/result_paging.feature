@cassandra-version-2.0
Feature: Result paging

  Starting with Cassandra native protocol v2 (used by Apache Cassandra 2.0),
  paging through query results is allowed.

  Page size can be specified by setting the `$pageSize` attribute of
  execution-options or cluster-wide, using `Dse\Cluster\Builder::withDefaultPageSize()`.

  Once a `Dse\Rows` object has been received, next page can be retrieved
  using `Dse\Rows::nextPage()` or `Dse\Rows::nextPageAsync()`
  methods for synchronous and asynchronous next page retrieval accordingly.

  To check if the next page is available, use `Dse\Rows::isLastPage()`

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TABLE paging_entries (key text, value int, PRIMARY KEY(key, value));
      INSERT INTO paging_entries (key, value) VALUES ('a', 0);
      INSERT INTO paging_entries (key, value) VALUES ('b', 1);
      INSERT INTO paging_entries (key, value) VALUES ('c', 2);
      INSERT INTO paging_entries (key, value) VALUES ('d', 3);
      INSERT INTO paging_entries (key, value) VALUES ('e', 4);
      INSERT INTO paging_entries (key, value) VALUES ('f', 5);
      INSERT INTO paging_entries (key, value) VALUES ('g', 6);
      INSERT INTO paging_entries (key, value) VALUES ('h', 7);
      INSERT INTO paging_entries (key, value) VALUES ('i', 8);
      INSERT INTO paging_entries (key, value) VALUES ('j', 9);
      INSERT INTO paging_entries (key, value) VALUES ('k', 10);
      INSERT INTO paging_entries (key, value) VALUES ('l', 11);
      INSERT INTO paging_entries (key, value) VALUES ('m', 12);
      """

  Scenario: Paging through results synchronously
    Given the following example:
      """php
      $cluster   = Dse::cluster()->build();
      $session   = $cluster->connect("simplex");
      $options   = array('page_size' => 5);
      $rows      = $session->execute("SELECT * FROM paging_entries", $options);

      while (true) {
          echo "Entries in Page: {$rows->count()}" . PHP_EOL;
          foreach ($rows as $row) {
              echo "  key: {$row['key']}, value: {$row['value']}" . PHP_EOL;
          }

          if ($rows->isLastPage()) {
              break;
          }
          $rows = $rows->nextPage();
      }
      """
    When it is executed
    Then its output should contain:
      """
      Entries in Page: 5
        key: a, value: 0
        key: c, value: 2
        key: m, value: 12
        key: f, value: 5
        key: g, value: 6
      Entries in Page: 5
        key: e, value: 4
        key: d, value: 3
        key: h, value: 7
        key: l, value: 11
        key: j, value: 9
      Entries in Page: 3
        key: i, value: 8
        key: k, value: 10
        key: b, value: 1
      """

  Scenario: Accessing page info after loading next one
    Given the following example:
      """php
      $cluster   = Dse::cluster()->build();
      $session   = $cluster->connect("simplex");

      $firstPageRows = $session->execute("SELECT * FROM paging_entries", array('page_size' => 10));
      echo "Page 1: " . ($firstPageRows->isLastPage() ? "last" : "not last") . PHP_EOL;

      $secondPageRows = $firstPageRows->nextPage();
      echo "Page 1: " . ($firstPageRows->isLastPage() ? "last" : "not last") . PHP_EOL;
      echo "Page 2: " . ($secondPageRows->isLastPage() ? "last" : "not last") . PHP_EOL;

      echo "Entries in Page 1: {$firstPageRows->count()}" . PHP_EOL;
      echo "Entries in Page 2: {$secondPageRows->count()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Page 1: not last
      Page 1: not last
      Page 2: last
      Entries in Page 1: 10
      Entries in Page 2: 3
      """

  @paging_state_token
  Scenario: Use paging state token to get next result
    Given the following example:
      """php
      $cluster   = Dse::cluster()->build();
      $session   = $cluster->connect("simplex");
      $query     = "SELECT * FROM paging_entries";
      $options = array('page_size' => 2);
      $result = $session->execute($query, $options);

      foreach ($result as $row) {
          echo "key: {$row['key']}, value: {$row['value']}" . PHP_EOL;
      }

      while ($result->pagingStateToken()) {
          $options = array(
              'page_size' => 2,
              'paging_state_token' => $result->pagingStateToken()
          );

          $result = $session->execute($query, $options);

          foreach ($result as $row) {
            echo "key: {$row['key']}, value: {$row['value']}" . PHP_EOL;
          }
      }
      """
    When it is executed
    Then its output should contain:
      """
      key: a, value: 0
      key: c, value: 2
      key: m, value: 12
      key: f, value: 5
      key: g, value: 6
      key: e, value: 4
      key: d, value: 3
      key: h, value: 7
      key: l, value: 11
      key: j, value: 9
      key: i, value: 8
      key: k, value: 10
      key: b, value: 1
      """
