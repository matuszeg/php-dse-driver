@cassandra-version-2.2
Feature: User-defined Function (UDF) and Aggregate Metadata (UDA)

  PHP Driver exposes the Cassandra Schema Metadata for UDFs and UDAs.

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      } AND DURABLE_WRITES = false;
      USE simplex;
      CREATE OR REPLACE FUNCTION fLog (input double) CALLED ON NULL INPUT RETURNS double LANGUAGE java AS 'return Double.valueOf(Math.log(input.doubleValue()));';
      CREATE OR REPLACE FUNCTION avgState ( state tuple<int,bigint>, val int ) CALLED ON NULL INPUT RETURNS tuple<int,bigint> LANGUAGE java AS 'if (val !=null) { state.setInt(0, state.getInt(0)+1); state.setLong(1, state.getLong(1)+val.intValue()); } return state;';
      CREATE OR REPLACE FUNCTION avgFinal ( state tuple<int,bigint> ) CALLED ON NULL INPUT RETURNS double LANGUAGE java AS 'double r = 0; if (state.getInt(0) == 0) return null; r = state.getLong(1); r/= state.getInt(0); return Double.valueOf(r);';
      CREATE AGGREGATE IF NOT EXISTS average ( int ) SFUNC avgState STYPE tuple<int,bigint> FINALFUNC avgFinal INITCOND (0,0);
      """

  Scenario: Getting a function's metadata
    Given the following example:
      """php
      $cluster = Dse::cluster()
                        ->withContactPoints('127.0.0.1')
                        ->build();
      $session = $cluster->connect("simplex");
      $schema = $session->schema();

      $function = $schema->keyspace("simplex")->function("flog", Dse\Type::double());

      echo "Name => {$function->simpleName()}" . PHP_EOL;
      echo "Signature => {$function->signature()}" . PHP_EOL;
      echo "Language => {$function->language()}" . PHP_EOL;
      echo "Body => {$function->body()}" . PHP_EOL;
      echo "Arguments:" . PHP_EOL;
      foreach ($function->arguments() as $name => $type) {
          echo "  {$name} => {$type}" . PHP_EOL;
      }
      echo "Return Type => {$function->returnType()}" . PHP_EOL;
      echo "Is Called On Null Input => " . ($function->isCalledOnNullInput() ? "true" : "false") . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Name => flog
      Signature => flog(double)
      Language => java
      Body => return Double.valueOf(Math.log(input.doubleValue()));

      Arguments:
        input => double
      Return Type => double
      Is Called On Null Input => true
      """

  Scenario: Getting an aggregates's metadata
    Given the following example:
      """php
      $cluster = Dse::cluster()
                        ->withContactPoints('127.0.0.1')
                        ->build();
      $session = $cluster->connect("simplex");
      $schema = $session->schema();

      $aggregate = $schema->keyspace("simplex")->aggregate("average", Dse\Type::int());

      echo "Name => {$aggregate->simpleName()}" . PHP_EOL;
      echo "Signature => {$aggregate->signature()}" . PHP_EOL;
      echo "Argument Types:" . PHP_EOL;
      foreach ($aggregate->argumentTypes() as $index => $type) {
          echo "  {$index} => {$type}" . PHP_EOL;
      }
      echo "State Type => {$aggregate->stateType()}" . PHP_EOL;
      echo "Return Type => {$aggregate->returnType()}" . PHP_EOL;
      echo "Initial Condition: {$aggregate->initialCondition()}" . PHP_EOL;
      echo "State Function: {$aggregate->stateFunction()->signature()}" . PHP_EOL;
      echo "Final Function: {$aggregate->finalFunction()->signature()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Name => average
      Signature => average(int)
      Argument Types:
        0 => int
      State Type => tuple<int, bigint>
      Return Type => double
      Initial Condition: (0, 0)
      State Function: avgstate(frozen<tuple<int,bigint>>,int)
      Final Function: avgfinal(frozen<tuple<int,bigint>>)
      """
