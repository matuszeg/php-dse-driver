@date_range
@dse-version-5.1
Feature: DSE DateRange Type

  DSE 5.1 introduced the DateRange type to represent ranges of dates
  and times. Instances of the `DateRange` type can be bound to queries
  and returned in columns in your application.

  Background:
    Given a running DSE cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': 1 };
      USE simplex;
      CREATE TABLE dr (k text PRIMARY KEY, v 'DateRangeType');
      """

  Scenario: Create a date-range using text embedded in the query string
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $session->execute("INSERT INTO dr (k, v) VALUES " .
        "('inline create', '[1971-01-02 TO 1972-01-03T06:00:00.234]')");

      $row = $session->execute("SELECT * FROM dr WHERE k = 'inline create'")->first();

      $dr = $row['v'];
      echo "As string: " . $dr . PHP_EOL;
      echo "Lower bound precision: {$dr->lowerBound()->precision()}" . PHP_EOL;
      echo "Lower bound timestamp: {$dr->lowerBound()->timeMs()}" . PHP_EOL;
      echo "Upper bound precision: {$dr->upperBound()->precision()}" . PHP_EOL;
      echo "Upper bound timestamp: {$dr->upperBound()->timeMs()}" . PHP_EOL;
      $single = $dr->isSingleDate() ? "true" : "false";
      echo "Single date: {$single}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      As string: 1971-01-02 00:00:00.000(DAY) TO 1972-01-03 06:00:00.234(MILLISECOND)
      Lower bound precision: 2
      Lower bound timestamp: 31622400000
      Upper bound precision: 6
      Upper bound timestamp: 63266400234
      Single date: false
      """

  Scenario: Create a date-range by binding a `DateRange` object
    Given the following example:
      """php
      use Dse\DateRange;
      use Dse\DateRange\Precision;
      use Dse\DateRange\Bound;

      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $bind_dr = new Dse\DateRange(Precision::DAY, new DateTime("1970-01-02Z"), Bound::unbounded());
      $session->execute("INSERT INTO dr (k, v) VALUES ('bind dr', ?)",
                        array("arguments" => array($bind_dr)));

      $row = $session->execute("SELECT * FROM dr WHERE k = 'bind dr'")->first();

      $dr = $row['v'];
      echo "As string: " . $dr . PHP_EOL;
      echo "Lower bound precision: {$dr->lowerBound()->precision()}" . PHP_EOL;
      echo "Lower bound timestamp: {$dr->lowerBound()->timeMs()}" . PHP_EOL;
      echo "Upper bound precision: {$dr->upperBound()->precision()}" . PHP_EOL;
      echo "Upper bound timestamp: {$dr->upperBound()->timeMs()}" . PHP_EOL;
      $single = $dr->isSingleDate() ? "true" : "false";
      echo "Single date: {$single}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      As string: 1970-01-02 00:00:00.000(DAY) TO *
      Lower bound precision: 2
      Lower bound timestamp: 86400000
      Upper bound precision: 255
      Upper bound timestamp: -1
      Single date: false
      """
  Scenario: Create a date-range with a single date
    Given the following example:
      """php
      use Dse\DateRange;
      use Dse\DateRange\Precision;

      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      // timestamp (ms) for 1/1/2017 midnight.
      // We could use a number literal on 64-bit platforms, but we need to use a string or Bigint on 32-bit.
      $t = new Dse\Bigint("1483228800000");

      $bind_dr = new Dse\DateRange(Precision::DAY, $t);
      $session->execute("INSERT INTO dr (k, v) VALUES ('single date', ?)",
                        array("arguments" => array($bind_dr)));

      $row = $session->execute("SELECT * FROM dr WHERE k = 'single date'")->first();

      $dr = $row['v'];
      echo "As string: " . $dr . PHP_EOL;
      echo "Lower bound precision: {$dr->lowerBound()->precision()}" . PHP_EOL;
      echo "Lower bound timestamp: {$dr->lowerBound()->timeMs()}" . PHP_EOL;
      echo "Upper bound: {$dr->upperBound()}" . PHP_EOL;
      $single = $dr->isSingleDate() ? "true" : "false";
      echo "Single date: {$single}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      As string: 2017-01-01 00:00:00.000(DAY)
      Lower bound precision: 2
      Lower bound timestamp: 1483228800000
      Upper bound:
      Single date: true
      """
  Scenario: Create a date-range with a single date in a Bound object
    Given the following example:
      """php
      use Dse\DateRange;
      use Dse\DateRange\Precision;
      use Dse\DateRange\Bound;

      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $bound = new Bound(Precision::DAY, new DateTime("1970-01-02Z"));
      $bind_dr = new Dse\DateRange($bound);
      $session->execute("INSERT INTO dr (k, v) VALUES ('single date in bound', ?)",
                        array("arguments" => array($bind_dr)));

      $row = $session->execute("SELECT * FROM dr WHERE k = 'single date in bound'")->first();

      $dr = $row['v'];
      echo "As string: " . $dr . PHP_EOL;
      echo "Lower bound precision: {$dr->lowerBound()->precision()}" . PHP_EOL;
      echo "Lower bound timestamp: {$dr->lowerBound()->timeMs()}" . PHP_EOL;
      echo "Upper bound: {$dr->upperBound()}" . PHP_EOL;
      $single = $dr->isSingleDate() ? "true" : "false";
      echo "Single date: {$single}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      As string: 1970-01-02 00:00:00.000(DAY)
      Lower bound precision: 2
      Lower bound timestamp: 86400000
      Upper bound:
      Single date: true
      """
