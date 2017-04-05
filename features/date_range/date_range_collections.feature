@date_range
@dse-version-5.1
Feature: DSE DateRange Type in Collections

  PHP Driver supports the `DateRange` datatype in collections

  Background:
    Given a running DSE cluster

  Scenario: Use the DateRange type in collections
    Given the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE simplex;
      CREATE TYPE date_range_user_type (range1 'DateRangeType', range2 'DateRangeType');
      CREATE TABLE date_ranges (k text PRIMARY KEY,
                              range_list list<'DateRangeType'>,
                              range_tuple tuple<'DateRangeType', 'DateRangeType'>,
                              range_udt date_range_user_type);
      """
    And the following example:
      """php
      use Dse\DateRange\Precision;
      use Dse\DateRange\Bound;

      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      # Construct collection arguments
      $range1 = new Dse\DateRange(Precision::DAY, new DateTime("1970-01-02Z"), Bound::unbounded());
      $range2 = new Dse\DateRange(Bound::unbounded(), Precision::DAY, new DateTime("1970-01-02Z"));

      $list = Dse\Type::collection(Dse\Type::dateRange())->create(
          $range1
      );
      $list->add($range2);

      $tuple = Dse\Type::tuple(Dse\Type::dateRange(), Dse\Type::dateRange())->create(
          $range1, $range2
      );

      $udt = Dse\Type::userType('range1', Dse\Type::dateRange(), 'range2', Dse\Type::dateRange())->create(
          'range1', $range1
      );
      $udt->set('range2', $range2);

      # Insert collections containing DateRange's into table
      $options = array('arguments' =>
          array('key1', $list, $tuple, $udt)
      );
      $session->execute("INSERT INTO date_ranges
          (k, range_list, range_tuple, range_udt) VALUES
          (?, ?, ?, ?)", $options);
      # Select collections from table and print
      $rows = $session->execute("SELECT * FROM date_ranges WHERE k = 'key1'");
      $row = $rows->first();


      $rows = $session->execute("SELECT * FROM date_ranges WHERE k = 'key1'");
      $row = $rows->first();

      echo 'The list contains ranges: [' . implode(', ', $row['range_list']->values()) . ']' . PHP_EOL;
      echo 'The tuple contains ranges: (' . implode(', ', $row['range_tuple']->values()) . ')' . PHP_EOL;
      $asString = array_map(function($k, $v) { return $k . ': ' . $v; },
          array_keys($row["range_udt"]->values()), $row["range_udt"]->values());
      echo 'The udt contains ranges: {' . implode(', ', $asString) . '}' . PHP_EOL;
      """
    When it is executed
    Then its output should contain disregarding order:
      """
The list contains ranges: [1970-01-02 00:00:00.000(DAY) TO *, * TO 1970-01-02 23:59:59.999(DAY)]
The tuple contains ranges: (1970-01-02 00:00:00.000(DAY) TO *, * TO 1970-01-02 00:00:00.000(DAY))
The udt contains ranges: {range1: 1970-01-02 00:00:00.000(DAY) TO *, range2: * TO 1970-01-02 00:00:00.000(DAY)}
      """

