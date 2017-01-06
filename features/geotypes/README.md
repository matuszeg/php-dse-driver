# Geospatial types

DataStax Enterprise 5.0+ adds types for representing geospatial data. The three
new geospatial types are point, line string and polygon. These types can be
specified directly in a query string using [well-known text (WKT)] or can be
bound to a query using the following objects:
[`Dse\Point`](/api/Dse/class.Point/),
[`Dse\LineString`](/api/Dse/class.LineString/) and
[`Dse\Polygon`](/api/Dse/class.Point/).


## Point

A point is a 2-dimensional location in space represented by two, double
precision floating numbers. Tables with columns of this type are created using
the type `org.apache.cassandra.db.marshal.PointType` or the shorter `PointType`.

```php
$session = Dse::cluster()->build()->connect("examples");

# Create table and add a coordinate for the point of interest using the `Point`
# type
$session->execute(new Dse\SimpleStatement("CREATE TABLE IF NOT EXISTS points_of_interest
                                           (name varchar PRIMARY KEY, coords 'PointType')"));

$session->execute(new Dse\SimpleStatement("INSERT INTO points_of_interest (name, coords) VALUES (?, ?)"),
                  new Dse\ExecutionOptions(array("arguments" => array("Eiffel Tower", new Dse\Point(48.8582, 2.2945)))));

# Get the coordinates for the inserted location
$row = $session->execute(new Dse\SimpleStatement("SELECT * FROM
points_of_interest"))->first();
echo "Name: '{$row['name']}' Coords: ({$row['coords']})" . PHP_EOL;
```

## Line String

A line string is a 2-dimensional set of lines represented by a string of points.
Tables with columns of this type are created using the type
`org.apache.cassandra.db.marshal.LineStringType` or the shorter
`LineStringType`.

```php
# Create table for trail paths
$session->execute(new Dse\SimpleStatement("CREATE TABLE IF NOT EXISTS trails
                                           (name varchar PRIMARY KEY, path 'LineStringType')"));

# Add a new trail path using the `LineString` type
$path = new Dse\LineString(new Dse\Point(38, 78), new Dse\Point(39, 78), new Dse\Point(39.5, 79)); # ...
$session->execute(new Dse\SimpleStatement("INSERT INTO trails (name, path) VALUES (?, ?)"),
                  new Dse\ExecutionOptions(array("arguments" => array("Appalachian National Scenic Trail", $path))));

# Get the trail's path coordinates
$row = $session->execute(new Dse\SimpleStatement("SELECT * FROM trails"))->first();
echo "Name: '{$row['name']}'" . PHP_EOL;
echo "Path: " . implode(" --> ", $row['path']->points()) . PHP_EOL;
```

## Polygon

A polygon is a bound set of line segments that form a closed loop. It is
characterized by a set of rings formed by line strings. The first ring
represents the external bounds of the polygon and all follow up rings represent
holes. The rings must be closed loops so they must be at least four points with
the first and last points being the same location.  Tables with columns of this
type are created using the type `org.apache.cassandra.db.marshal.PolygonType`
or the shorter `PolygonType`.

```php
# Create a table to represent boundaries of places in the world (in this case a state)
$session->execute(new Dse\SimpleStatement("CREATE TABLE IF NOT EXISTS boundaries
                                           (name varchar PRIMARY KEY, boundary 'PolygonType')"));

# Use a `LineString` to represent the exterior boundary of the state
$stateBoundary = new Dse\LineString(new Dse\Point(35, 10),
                                    new Dse\Point(45, 45),
                                    new Dse\Point(15, 40),
                                    new Dse\Point(10, 20),
                                    new Dse\Point(35, 10));

# Use a `LineString` to represent the boundary of a county inside the state's
# exteriro boundary
$countyBoundary = new Dse\LineString(new Dse\Point(20, 30),
                                     new Dse\Point(35, 35),
                                     new Dse\Point(30, 20),
                                     new Dse\Point(20, 30));

# Add the state's boundary using the `Polygon` type
$boundary = new Dse\Polygon($stateBoundary, $countyBoundary);
$session->execute(new Dse\SimpleStatement("INSERT INTO boundaries (name, boundary) VALUES (?, ?)"),
                  new Dse\ExecutionOptions(array("arguments" => array("California", $boundary))));

$ Get the state's boundary
$row = $session->execute(new Dse\SimpleStatement("SELECT * FROM boundaries"))->first();
echo "Name: '{$row['name']}'" . PHP_EOL;
echo "State Boundary: " . implode("; ", $row['boundary']->exteriorRing()->points()) . PHP_EOL;
foreach ($row['boundary']->interiorRings() as $countyBoundary) {
    echo "County Boundary: " . implode("; ", $countyBoundary->points()) . PHP_EOLo
}
```

[well-known text (WKT)]: https://en.wikipedia.org/wiki/Well-known_text
