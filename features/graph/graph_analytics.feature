@graph
@dse-version-5.0
Feature: DSE Geospatial Types

  DSE 5.0 included a graph-oriented database queried utilizing the Gremlin
  graph language. Graph queries are executed using
  `Dse\Session::executeGraph()` or `Dse\Session::executeGraphAsync()`.

  Background:
    Given a running DSE cluster with graph and spark
    And an existing graph called "user_analytics" with schema:
      """gremlin
      schema.propertyKey('name').Text().ifNotExists().create();
      schema.propertyKey('age').Int().ifNotExists().create();
      schema.propertyKey('lang').Text().ifNotExists().create();
      schema.propertyKey('weight').Float().ifNotExists().create();
      schema.vertexLabel('person').properties('name', 'age').ifNotExists().create();
      schema.vertexLabel('software').properties('name', 'lang').ifNotExists().create();
      schema.edgeLabel('created').properties('weight').connection('person', 'software').ifNotExists().create();
      schema.edgeLabel('knows').properties('weight').connection('person', 'person').ifNotExists().create();
      schema.propertyKey('country').Text().ifNotExists().create();
      schema.propertyKey('origin').Text().multiple().properties('country').ifNotExists().create();
      schema.vertexLabel('master').properties('name', 'origin').ifNotExists().create();
      schema.vertexLabel('character').properties('name').ifNotExists().create();

      Vertex marko = graph.addVertex(label, 'person', 'name', 'marko', 'age', 29);
      Vertex vadas = graph.addVertex(label, 'person', 'name', 'vadas', 'age', 27);
      Vertex lop = graph.addVertex(label, 'software', 'name', 'lop', 'lang', 'java');
      Vertex josh = graph.addVertex(label, 'person', 'name', 'josh', 'age', 32);
      Vertex ripple = graph.addVertex(label, 'software', 'name', 'ripple', 'lang', 'java');
      Vertex peter = graph.addVertex(label, 'person', 'name', 'peter', 'age', 35);
      """

  Scenario: Running an OLAP graph query
    Given the following example:
      """php
      $graphOptions = Dse::graphOptions()
        ->withGraphName("user_analytics")
        ->build();

      $cluster = Dse::cluster()
        ->withGraphOptions($graphOptions)
        ->build();
      $session = $cluster->connect();

      $resultset = $session->executeGraph("g.V().count()", array("graph_source" => "a"));

      echo "Result: {$resultset->first()->value()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Result: 6
      """
