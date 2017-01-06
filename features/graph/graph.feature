@graph
@dse-version-5.0
Feature: DSE Geospatial Types

  DSE 5.0 included a graph-oriented database queried utilizing the Gremlin
  graph language. Graph queries are executed using
  `Dse\Session::executeGraph()` or `Dse\Session::executeGraphAsync()`.

  Background:
    Given a running DSE cluster with graph
    And an existing graph called "users" with schema:
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
      """

  Scenario: Create and query vertex with graph options
    Given the following example:
      """php
      $graphOptions = Dse::graphOptions()
        ->withGraphName("users")
        ->withGraphSource("g")
        ->withGraphLanguage("gremlin-groovy")
        ->withReadConsistency(Dse::CONSISTENCY_ONE)
        ->withWriteConsistency(Dse::CONSISTENCY_ONE)
        ->withRequestTimeout(1)
        ->build();

      $cluster = Dse::cluster()
        ->withGraphOptions($graphOptions)
        ->build();
      $session = $cluster->connect();

      $session->executeGraph("Vertex marko = graph.addVertex(label, 'person', 'name', 'marko', 'age', 29);");

      $resultset = $session->executeGraph("g.V().has('name', 'marko')[0]");
      $vertex = $resultset->first()->asVertex();

      echo "Vertex label: {$vertex->id()['~label']}" . PHP_EOL;

      foreach($vertex->properties() as $property) {
        echo "Vertex property name: {$property->name()}" . PHP_EOL;
        foreach ($property->value() as $value) {
          echo "Vertex property value: {$value['value']}" . PHP_EOL;
        }
      }
      """
    When it is executed
    Then its output should contain:
      """
      Vertex label: person
      Vertex property name: name
      Vertex property value: marko
      Vertex property name: age
      Vertex property value: 29
      """

  Scenario: Create and query edges
    Given the following example:
      """php
      $graphOptions = Dse::graphOptions()
        ->withGraphName("users")
        ->build();

      $cluster = Dse::cluster()
        ->withGraphOptions($graphOptions)
        ->build();
      $session = $cluster->connect();

      $session->executeGraph(
        "Vertex marko = graph.addVertex(label, 'person', 'name', 'marko', 'age', 29);
         Vertex vadas = graph.addVertex(label, 'person', 'name', 'vadas', 'age', 27);
         marko.addEdge('knows', vadas, 'weight', 0.5f);");

      $resultset = $session->executeGraph("g.V().has('name', 'marko').outE('knows')");
      $edge = $resultset->first()->asEdge();

      echo "Edge type: {$edge->id()['~type']}" . PHP_EOL;
      echo "Edge incoming vertex label: {$edge->inVLabel()}" . PHP_EOL;
      echo "Edge outgoing vertex label: {$edge->outVLabel()}" . PHP_EOL;

      foreach($edge->properties() as $property) {
        echo "Edge property name: {$property->name()}" . PHP_EOL;
        echo "Edge property value: {$property->value()}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain:
      """
      Edge type: knows
      Edge incoming vertex label: person
      Edge outgoing vertex label: person
      Edge property name: weight
      Edge property value: 0.5
      """

  Scenario: Using paths
    Given the following example:
      """php
      $graphOptions = Dse::graphOptions()
        ->withGraphName("users")
        ->build();

      $cluster = Dse::cluster()
        ->withGraphOptions($graphOptions)
        ->build();
      $session = $cluster->connect();

      $session->executeGraph(
        "Vertex marko = graph.addVertex(label, 'person', 'name', 'marko', 'age', 29);
         Vertex vadas = graph.addVertex(label, 'person', 'name', 'vadas', 'age', 27);
         Vertex lop = graph.addVertex(label, 'software', 'name', 'lop', 'lang', 'java');
         Vertex josh = graph.addVertex(label, 'person', 'name', 'josh', 'age', 32);
         Vertex ripple = graph.addVertex(label, 'software', 'name', 'ripple', 'lang', 'java');
         Vertex peter = graph.addVertex(label, 'person', 'name', 'peter', 'age', 35);
         marko.addEdge('knows', vadas, 'weight', 0.5f);
         marko.addEdge('knows', josh, 'weight', 1.0f);
         marko.addEdge('created', lop, 'weight', 0.4f);
         josh.addEdge('created', ripple, 'weight', 1.0f);
         josh.addEdge('created', lop, 'weight', 0.4f);
         peter.addEdge('created', lop, 'weight', 0.2f);");


      $resultset = $session->executeGraph(
        "g.V().hasLabel('person').has('name', 'marko').as('a').outE('knows').inV().as('c', 'd').outE('created').as('e', 'f', 'g').inV().path()");

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
      """
    When it is executed
    Then its output should contain:
      """
      Path labels: [["a"],[],["c","d"],["e","f","g"],[]]
      Length of path: 5
      Object 0 is a(n) Dse\Graph\DefaultVertex with the value: marko
      Object 1 is a(n) Dse\Graph\DefaultEdge with the value: knows
      Object 2 is a(n) Dse\Graph\DefaultVertex with the value: josh
      Object 3 is a(n) Dse\Graph\DefaultEdge with the value: created
      Object 4 is a(n) Dse\Graph\DefaultVertex with the value: lop
      """

  Scenario: Inspecting a simple result
    Given the following example:
      """php
      $graphOptions = Dse::graphOptions()
        ->withGraphName("users")
        ->build();

      $cluster = Dse::cluster()
        ->withGraphOptions($graphOptions)
        ->build();
      $session = $cluster->connect();

      $resultset = $session->executeGraph("g.V().has('name', 'marko')[0].property('age').value()");
      $result = $resultset->first();

      echo "The result type is a number? " . ($result->isNumber() ? "true" : "false") . PHP_EOL;
      echo "The result value is: {$result}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      The result type is a number? true
      The result value is: 29
      """

