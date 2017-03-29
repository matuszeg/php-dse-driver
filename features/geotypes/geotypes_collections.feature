@geotypes
@dse-version-5.0
Feature: DSE Geospatial Types in Collections

  DSE 5.0 introduced geospatial types. The `Point`, `LineString` and `Polygon`
  geospatial type objects are used to represent these types in your
  application and can be added to all the existing collection types.

  Background:
    Given a running DSE cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': 1 };
      USE simplex;

      CREATE TYPE point_user_type (point1 'PointType', point2 'PointType');
      CREATE TABLE points (k text PRIMARY KEY,
                           point_list list<'PointType'>,
                           point_set set<'PointType'>,
                           point_map map<'PointType', 'PointType'>,
                           point_tuple tuple<'PointType', 'PointType'>,
                           point_udt frozen<point_user_type>);

      CREATE TYPE linestring_user_type (linestring1 'LineStringType', linestring2 'LineStringType');
      CREATE TABLE linestrings (k text PRIMARY KEY,
                                linestring_list list<'LineStringType'>,
                                linestring_set set<'LineStringType'>,
                                linestring_map map<'LineStringType', 'LineStringType'>,
                                linestring_tuple tuple<'LineStringType', 'LineStringType'>,
                                linestring_udt frozen<linestring_user_type>);

      CREATE TYPE polygon_user_type (polygon1 'PolygonType', polygon2 'PolygonType');
      CREATE TABLE polygons (k text PRIMARY KEY,
                             polygon_list list<'PolygonType'>,
                             polygon_set set<'PolygonType'>,
                             polygon_map map<'PolygonType', 'PolygonType'>,
                             polygon_tuple tuple<'PolygonType', 'PolygonType'>,
                             polygon_udt frozen<polygon_user_type>);
      """

  Scenario: Add the `Point` geospatial type to various collections
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      # Construct collection arguments

      $list = Dse\Type::collection(Dse\Type::point())->create(
        new Dse\Point(0.0, 1.0),
        new Dse\Point(2.0, 3.0)
      );
      $list->add(new Dse\Point(4.0, 5.0));

      $set = Dse\Type::set(Dse\Type::point())->create(
        new Dse\Point(0.0, 1.0),
        new Dse\Point(2.0, 3.0)
      );
      $set->add(new Dse\Point(4.0, 5.0));

      $map = Dse\Type::map(Dse\Type::point(), Dse\Type::point())->create(
        new Dse\Point(1.0, 2.0), new Dse\Point(3.0, 4.0),
        new Dse\Point(5.0, 6.0), new Dse\Point(7.0, 8.0)
      );
      $map->set(new Dse\Point(9.0, 10.0), new Dse\Point(11.0, 12.0));

      $tuple = Dse\Type::tuple(Dse\Type::point(), Dse\Type::point())->create(
        new Dse\Point(1.0, 2.0), new Dse\Point(3.0, 4.0)
      );

      $pointUserType = $session->schema()->keyspace("simplex")->userType("point_user_type");

      $udt = $pointUserType->create('point1', new Dse\Point(1.0, 2.0));
      $udt->set('point2', new Dse\Point(3.0, 4.0));

      # Insert collections containing points into table

      $options = array('arguments' =>
        array('key1', $list, $set, $map, $tuple, $udt)
      );
      $session->execute("INSERT INTO points
                         (k, point_list, point_set, point_map, point_tuple, point_udt) VALUES
                         (?, ?, ?, ?, ?, ?)", $options);

      # Select collections from table and print as WKT

      $rows = $session->execute("SELECT * FROM points WHERE k = 'key1'");
      $row = $rows->first();

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['point_list']->values());
      echo 'The list contains points: [' . implode(', ', $asWkt) . ']' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['point_set']->values());
      echo 'The set contains points: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k->wkt() . ': ' . $v->wkt(); },
                               $row["point_map"]->keys(), $row["point_map"]->values());
      echo 'The map contains points: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['point_tuple']->values());
      echo 'The tuple contains points: (' . implode(', ', $asWkt) . ')' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k . ': ' . $v->wkt(); },
                               array_keys($row["point_udt"]->values()), $row["point_udt"]->values());
      echo 'The udt contains points: {' . implode(', ', $asWkt) . '}' . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      The list contains points: [POINT (0 1), POINT (2 3), POINT (4 5)]
      The set contains points: {POINT (0 1), POINT (2 3), POINT (4 5)}
      The map contains points: {POINT (5 6): POINT (7 8), POINT (9 10): POINT (11 12), POINT (1 2): POINT (3 4)}
      The tuple contains points: (POINT (1 2), POINT (3 4))
      The udt contains points: {point1: POINT (1 2), point2: POINT (3 4)}
      """

  Scenario: Add the `LineString` geospatial type to various collections
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      # Our line strings

      $linestring1 = new Dse\LineString(new Dse\Point(2, 3),
                                        new Dse\Point(5, 5),
                                        new Dse\Point(3, 2),
                                        new Dse\Point(2, 3));

      $linestring2 =  new Dse\LineString("LINESTRING (1 2, 4 4, 2 1, 1 2)");

      $linestring3 = new Dse\LineString(new Dse\Point(0, 0),
                                        new Dse\Point(2, 2),
                                        new Dse\Point(1, 2));

      # Construct collection arguments

      $list = Dse\Type::collection(Dse\Type::lineString())->create($linestring1, $linestring2);
      $list->add($linestring3);

      $set = Dse\Type::set(Dse\Type::lineString())->create($linestring1, $linestring2);
      $set->add($linestring3);

      $map = Dse\Type::map(Dse\Type::lineString(), Dse\Type::lineString())->create(
          $linestring1, $linestring2,
          $linestring2, $linestring3
      );
      $map->set($linestring3, $linestring1);

      $tuple = Dse\Type::tuple(Dse\Type::lineString(), Dse\Type::lineString())->create($linestring1, $linestring2);

      $lineStringUserType = $session->schema()->keyspace("simplex")->userType("linestring_user_type");

      $udt = $lineStringUserType->create(
        'linestring1', $linestring1
      );
      $udt->set('linestring2', $linestring2);

      # Insert collections containing line strings into table

      $options = array('arguments' =>
        array('key1', $list, $set, $map, $tuple, $udt)
      );
      $session->execute("INSERT INTO linestrings
                         (k, linestring_list, linestring_set, linestring_map, linestring_tuple, linestring_udt) VALUES
                         (?, ?, ?, ?, ?, ?)", $options);

      # Select collections from table and print as WKT

      $rows = $session->execute("SELECT * FROM linestrings WHERE k = 'key1'");
      $row = $rows->first();

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['linestring_list']->values());
      echo 'The list contains line strings: [' . implode(', ', $asWkt) . ']' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['linestring_set']->values());
      echo 'The set contains line strings: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k->wkt() . ': ' . $v->wkt(); },
                               $row["linestring_map"]->keys(), $row["linestring_map"]->values());
      echo 'The map contains line strings: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['linestring_tuple']->values());
      echo 'The tuple contains line strings: (' . implode(', ', $asWkt) . ')' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k . ': ' . $v->wkt(); },
                         array_keys($row["linestring_udt"]->values()), $row["linestring_udt"]->values());
      echo 'The udt contains line strings: {' . implode(', ', $asWkt) . '}' . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      The list contains line strings: [LINESTRING (2 3, 5 5, 3 2, 2 3), LINESTRING (1 2, 4 4, 2 1, 1 2), LINESTRING (0 0, 2 2, 1 2)]
      The set contains line strings: {LINESTRING (0 0, 2 2, 1 2), LINESTRING (2 3, 5 5, 3 2, 2 3), LINESTRING (1 2, 4 4, 2 1, 1 2)}
      The map contains line strings: {LINESTRING (0 0, 2 2, 1 2): LINESTRING (2 3, 5 5, 3 2, 2 3), LINESTRING (2 3, 5 5, 3 2, 2 3): LINESTRING (1 2, 4 4, 2 1, 1 2), LINESTRING (1 2, 4 4, 2 1, 1 2): LINESTRING (0 0, 2 2, 1 2)}
      The tuple contains line strings: (LINESTRING (2 3, 5 5, 3 2, 2 3), LINESTRING (1 2, 4 4, 2 1, 1 2))
      The udt contains line strings: {linestring1: LINESTRING (2 3, 5 5, 3 2, 2 3), linestring2: LINESTRING (1 2, 4 4, 2 1, 1 2)}
      """

  Scenario: Add the `Polygon` geospatial type to various collections
    Given the following example:
      """php
      $cluster = Dse::cluster()->build();
      $session = $cluster->connect("simplex");

      # Our polygons

      $polygon1 = new Dse\Polygon(new Dse\LineString(new Dse\Point(20, 30),
                                                     new Dse\Point(30, 20),
                                                     new Dse\Point(35, 35),
                                                     new Dse\Point(20, 30)));

      $polygon2 = new Dse\Polygon("POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10))");

      $polygon3 = new Dse\Polygon(new Dse\LineString(new Dse\Point(35, 10),
                                                     new Dse\Point(45, 45),
                                                     new Dse\Point(15, 40),
                                                     new Dse\Point(10, 20),
                                                     new Dse\Point(35, 10)),
                                  new Dse\LineString(new Dse\Point(20, 30),
                                                     new Dse\Point(35, 35),
                                                     new Dse\Point(30, 20),
                                                     new Dse\Point(20, 30)));

      # Construct collection arguments

      $list = Dse\Type::collection(Dse\Type::polygon())->create($polygon1, $polygon2);
      $list->add($polygon3);

      $set = Dse\Type::set(Dse\Type::polygon())->create($polygon1, $polygon2);
      $set->add($polygon3);


      $map = Dse\Type::map(Dse\Type::polygon(), Dse\Type::polygon())->create(
          $polygon1, $polygon2,
          $polygon2, $polygon3
      );
      $map->set($polygon3, $polygon1);

      $tuple = Dse\Type::tuple(Dse\Type::polygon(), Dse\Type::polygon())->create($polygon1, $polygon2);

      $polygonUserType = $session->schema()->keyspace("simplex")->userType("polygon_user_type");

      $udt = $polygonUserType->create('polygon1', $polygon1);
      $udt->set('polygon2', $polygon2);

      # Insert collections containing line strings into table

      $options = array('arguments' =>
        array('key1', $list, $set, $map, $tuple, $udt)
      );
      $session->execute("INSERT INTO polygons
                         (k, polygon_list, polygon_set, polygon_map, polygon_tuple, polygon_udt) VALUES
                         (?, ?, ?, ?, ?, ?)", $options);

      # Select collections from table and print as WKT

      $rows = $session->execute("SELECT * FROM polygons WHERE k = 'key1'");
      $row = $rows->first();

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['polygon_list']->values());
      echo 'The list contains polygons: [' . implode(', ', $asWkt) . ']' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['polygon_set']->values());
      echo 'The set contains polygons: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k->wkt() . ': ' . $v->wkt(); },
                               $row["polygon_map"]->keys(), $row["polygon_map"]->values());
      echo 'The map contains polygons: {' . implode(', ', $asWkt) . '}' . PHP_EOL;

      $asWkt = array_map(function($p) { return $p->wkt(); }, $row['polygon_tuple']->values());
      echo 'The tuple contains polygons: (' . implode(', ', $asWkt) . ')' . PHP_EOL;

      $asWkt = array_map(function($k, $v) { return $k . ': ' . $v->wkt(); },
                         array_keys($row["polygon_udt"]->values()), $row["polygon_udt"]->values());
      echo 'The udt contains polygons: {' . implode(', ', $asWkt) . '}' . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      The list contains polygons: [POLYGON ((20 30, 30 20, 35 35, 20 30)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))]
      The set contains polygons: {POLYGON ((20 30, 30 20, 35 35, 20 30)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))}
      The map contains polygons: {POLYGON ((20 30, 30 20, 35 35, 20 30)): POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10)): POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30)): POLYGON ((20 30, 30 20, 35 35, 20 30))}
      The tuple contains polygons: (POLYGON ((20 30, 30 20, 35 35, 20 30)), POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10)))
      The udt contains polygons: {polygon1: POLYGON ((20 30, 30 20, 35 35, 20 30)), polygon2: POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10))}
      """
