@geotypes
@dse-version-5.0
Feature: DSE Geospatial Types

  DSE 5.0 introduced geospatial types. The `Point`, `LineString` and `Polygon`
  geospatial type objects are used to represent these types in your
  application. These objects can be bound to queries and returned in columns
  values.

  Background:
    Given a running DSE cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': 1 };
      USE simplex;
      CREATE TABLE points (k text PRIMARY KEY, v 'PointType');
      CREATE TABLE linestrings (k text PRIMARY KEY, v 'LineStringType');
      CREATE TABLE polygons (k text PRIMARY KEY, v 'PolygonType');
      """

  Scenario: Create a point using well-known text embedded in the query string
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $session->execute(new Dse\SimpleStatement("INSERT INTO points (k, v) VALUES ('point0', 'POINT (1.0 2.0)')"));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM points WHERE k = 'point0'"))->first();

      $point = $row['v'];
      echo "X coord: {$point->x()}" . PHP_EOL;
      echo "Y coord: {$point->y()}" . PHP_EOL;
      echo "Coords as a string: $point" . PHP_EOL;
      echo "Point as WKT: {$point->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      X coord: 1
      Y coord: 2
      Coords as a string: 1,2
      Point as WKT: POINT (1 2)
      """

  Scenario: Create a point by binding a `Point` object
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $point = new Dse\Point(1.0, 2.0);
      $session->execute(new Dse\SimpleStatement("INSERT INTO points (k, v) VALUES ('point1', ?)"),
                        new Dse\ExecutionOptions(array("arguments" => array($point))));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM points WHERE k = 'point1'"))->first();

      $point = $row['v'];
      echo "X coord: {$point->x()}" . PHP_EOL;
      echo "Y coord: {$point->y()}" . PHP_EOL;
      echo "Point as a string: $point" . PHP_EOL;
      echo "Point as WKT: {$point->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      X coord: 1
      Y coord: 2
      Point as a string: 1,2
      Point as WKT: POINT (1 2)
      """

  Scenario: Create a line string using well-known text embedded in the query string
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $session->execute(new Dse\SimpleStatement("INSERT INTO linestrings (k, v) VALUES ('linestring0', 'LINESTRING (0.0 1.0, 2.0 3.0, 4.0 5.0)')"));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM linestrings WHERE k = 'linestring0'"))->first();

      $linestring = $row['v'];
      echo "First point: {$linestring->point(0)}" . PHP_EOL;
      echo "Second point: {$linestring->point(1)}" . PHP_EOL;
      echo "Third point: {$linestring->point(2)}" . PHP_EOL;
      echo "LineString as a string: $linestring" . PHP_EOL;
      echo "LineString as WKT: {$linestring->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      First point: 0,1
      Second point: 2,3
      Third point: 4,5
      LineString as a string: 0,1 to 2,3 to 4,5
      LineString as WKT: LINESTRING (0 1, 2 3, 4 5)
      """

  Scenario: Create a line string by binding a `LineString` object
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $linestring = new Dse\LineString(new Dse\Point(0.0, 1.0),
                                       new Dse\Point(2.0, 3.0),
                                       new Dse\Point(4.0, 5.0));

      $session->execute(new Dse\SimpleStatement("INSERT INTO linestrings (k, v) VALUES ('linestring1', ?)"),
                        new Dse\ExecutionOptions(array("arguments" => array($linestring))));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM linestrings WHERE k = 'linestring1'"))->first();

      $linestring = $row['v'];
      echo "First point: {$linestring->point(0)}" . PHP_EOL;
      echo "Second point: {$linestring->point(1)}" . PHP_EOL;
      echo "Third point: {$linestring->point(2)}" . PHP_EOL;
      echo "LineString as a string: $linestring" . PHP_EOL;
      echo "LineString as WKT: {$linestring->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      First point: 0,1
      Second point: 2,3
      Third point: 4,5
      LineString as a string: 0,1 to 2,3 to 4,5
      LineString as WKT: LINESTRING (0 1, 2 3, 4 5)
      """

  Scenario: Create empty line string
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $linestring = new Dse\LineString();
      $session->execute(new Dse\SimpleStatement("INSERT INTO linestrings (k, v) VALUES ('linestring_empty', ?)"),
                        new Dse\ExecutionOptions(array("arguments" => array($linestring))));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM linestrings WHERE k = 'linestring_empty'"))->first();

      $linestring = $row['v'];
      echo "LineString as WKT: {$linestring->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      LineString as WKT: LINESTRING EMPTY
      """

  Scenario: Create a polygon using well-known text embedded in the query string
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $session->execute(new Dse\SimpleStatement("INSERT INTO polygons (k, v)
                                                VALUES ('polygon0',
                                                'POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10),
                                                          (20 30, 35 35, 30 20, 20 30))')"));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM polygons WHERE k = 'polygon0'"))->first();

      $polygon = $row['v'];
      echo "Exterior ring: {$polygon->exteriorRing()}" . PHP_EOL;
      echo "Interior ring: {$polygon->interiorRings()[0]}" . PHP_EOL;
      echo "Polygon as a string: " . PHP_EOL;
      echo $polygon . PHP_EOL;
      echo "Polygon as wkt {$polygon->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Exterior ring: 35,10 to 45,45 to 15,40 to 10,20 to 35,10
      Interior ring: 20,30 to 35,35 to 30,20 to 20,30
      Polygon as a string:
      Exterior ring: 35,10 to 45,45 to 15,40 to 10,20 to 35,10
      Interior rings:
          20,30 to 35,35 to 30,20 to 20,30
      Polygon as wkt POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))
      """

  Scenario: Create a polygon by binding a `Polygon` object
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $linestring0 = new Dse\LineString(new Dse\Point(35, 10),
                                        new Dse\Point(45, 45),
                                        new Dse\Point(15, 40),
                                        new Dse\Point(10, 20),
                                        new Dse\Point(35, 10));

      $linestring1 = new Dse\LineString(new Dse\Point(20, 30),
                                        new Dse\Point(35, 35),
                                        new Dse\Point(30, 20),
                                        new Dse\Point(20, 30));

      $polygon = new Dse\Polygon($linestring0, $linestring1);

      $session->execute(new Dse\SimpleStatement("INSERT INTO polygons (k, v) VALUES ('polygon1', ?)"),
                        new Dse\ExecutionOptions(array("arguments" => array($polygon))));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM polygons WHERE k = 'polygon1'"))->first();

      $polygon = $row['v'];
      echo "Exterior ring: {$polygon->exteriorRing()}" . PHP_EOL;
      echo "Interior ring: {$polygon->interiorRings()[0]}" . PHP_EOL;
      echo "Polygon as a string: " . PHP_EOL;
      echo $polygon . PHP_EOL;
      echo "Polygon as wkt {$polygon->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Exterior ring: 35,10 to 45,45 to 15,40 to 10,20 to 35,10
      Interior ring: 20,30 to 35,35 to 30,20 to 20,30
      Polygon as a string:
      Exterior ring: 35,10 to 45,45 to 15,40 to 10,20 to 35,10
      Interior rings:
          20,30 to 35,35 to 30,20 to 20,30
      Polygon as wkt POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))
      """

  Scenario: Create empty polygon
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      $polygon = new Dse\Polygon();
      $session->execute(new Dse\SimpleStatement("INSERT INTO polygons (k, v) VALUES ('polygon_empty', ?)"),
                        new Dse\ExecutionOptions(array("arguments" => array($polygon))));

      $row = $session->execute(new Dse\SimpleStatement("SELECT * FROM polygons WHERE k = 'polygon_empty'"))->first();

      $polygon = $row['v'];
      echo "Polygon as WKT: {$polygon->wkt()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Polygon as WKT: POLYGON EMPTY
      """
