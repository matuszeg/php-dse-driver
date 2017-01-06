# Graph

DataStax Enterprise 5.0+ now includes a graph-orient database. The DSE driver
can execute graph queries written in the Gremlin language. Graph queries are
executed using either the `Session::executeGraph()` and
`Session::executeGraphAsync()` methods and return `Graph\ResultSet` objects
which can represent Cassandra types or graph specific types (e.g. vertices and
edges).

## Graph Options

Graph options control how graph queries are run and can be specified when
constructing a `Dse\Cluster` object or they can be specified per query. At a
minimum, a graph name must be provide when running regular graph queries.
Running system commands do not require specifying a graph name.

```php
# Construct graph options with a graph name "users"
$graphOptions = Dse::graphOptions()
  ->withGraphName("users")
  ->build();

# Construct a new cluster object with specific graph options
$cluster = Dse::cluster()
  ->withGraphOptions($graphOptions)
  ->build();
$session = $cluster->connect();

# Execute a graph query using the cluster-level graph options
$resultset = $session->executeGraph("g.V().count()");
```

Graph options can also be specified (or overridden) when executing a query.

```php
$cluster = Dse::cluster()->build();
$session = $cluster->connect();

$resultset = $session->executeGraph("g.V().count()", array("graph_name" => "users"));
```

## Graph Results

A `Dse\Graph\ResultSet` object is returned from the graph execution methods.
Resultsets are iterable and indexable list of `Dse\Graph\Result` objects.
`Dse\Graph\Result` is an arbitrary data result and it can be various
types from simple data types such as numbers and strings, composite data types
such as arrays and dictionaries, as well as graph elements such as vertices and
edges.

```php
# A `Dse\Graph\Result can hold arbitrary data, in this case the number of
# vertices held by the graph.
$result = $session->execute("g.V().count()")->first();

echo "Vertex count: {$result->value()}" . PHP_EOL;
```

## Vertices

Vertices are graph elements connected by edges. They contain properties and
unlike other graph elements (e.g. edges)  vertex properties can contain multiple
values and can have properties of their own.

```php
$resultset = $session->executeGraph("g.V()");

foreach ($resultset as $result) {
  $vertex = $result->asVertex(); # Convert to vertex

  echo "Vertex label: {$vertex->id()['~label']}" . PHP_EOL;

  foreach($vertex->properties() as $property) {
    echo "Vertex property name: {$property->name()}" . PHP_EOL;
    # Each vertex property can contain multiple values
    foreach ($property->value() as $value) {
      echo "Vertex property value: {$value['value']}" . PHP_EOL;
    }
  }
}
```

## Edges

Edges connected pairs of vertices. Like vertices they have properties, but
they are simple key/value pairs.

```php
$resultset = $session->executeGraph("g.E()");

foreach ($resultset as $result) {
  $edge = $result->asEdge();

  echo "Edge type: {$edge->id()['~type']}" . PHP_EOL;

  # Each edge has an input and output vertex
  echo "Edge incoming vertex label: {$edge->inVLabel()}" . PHP_EOL;
  echo "Edge outgoing vertex label: {$edge->outVLabel()}" . PHP_EOL;

  # Edge properties are simple key/value pairs
  foreach($edge->properties() as $property) {
    echo "Edge property name: {$property->name()}" . PHP_EOL;
    echo "Edge property value: {$property->value()}" . PHP_EOL;
  }
}
```

## Paths

Paths describe a list of graph elements that connect two vertices.

```php
$resultset = $session->executeGraph("g.V().in().path()");

$path = $resultset->first()->asPath();

echo "Path labels: " . json_encode($path->labels()) . PHP_EOL;
echo "Length of path: " . count($path->labels()) . PHP_EOL;

$objects = $path->objects();
for ($i = 0; $i < count($objects); $i++) {
  try {
    $vertex = $objects[$i]->asVertex();
    echo "Object $i is a(n) " . get_class($vertex) . " with the value: " . $vertex->property('name')->value()[0]['value'] . PHP_EOL;
  } catch(Dse\Exception\DomainException $e) {
    $edge = $objects[$i]->asEdge();
    echo "Object $i is a(n) " . get_class($edge) . " with the value: " . $edge->label(). PHP_EOL;
  }
}
```

