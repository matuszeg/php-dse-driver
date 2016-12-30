Feature: Schema Metadata

  PHP Driver exposes the Cassandra Schema Metadata for keyspaces, tables, and
  columns.

  Background:
    Given a running Cassandra cluster
    And the following schema:
      """cql
      CREATE KEYSPACE simplex WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      } AND DURABLE_WRITES = false;
      USE simplex;
      CREATE TABLE values (
        id int PRIMARY KEY,
        bigint_value bigint,
        decimal_value decimal,
        double_value double,
        float_value float,
        int_value int,
        varint_value varint,
        ascii_value ascii,
        timestamp_value timestamp,
        blob_value blob,
        uuid_value uuid,
        timeuuid_value timeuuid,
        inet_value inet,
        list_value List<bigint>,
        map_value Map<timestamp, double>,
        set_value Set<float>
      ) WITH
          bloom_filter_fp_chance=0.5 AND
          comment='Schema Metadata Feature' AND
          compaction={'class': 'LeveledCompactionStrategy', 'sstable_size_in_mb' : 37} AND
          compression={'sstable_compression': 'DeflateCompressor'} AND
          dclocal_read_repair_chance=0.25 AND
          gc_grace_seconds=3600 AND
          read_repair_chance=0.75;
      """

  Scenario: Getting keyspace metadata
    Given the following example:
      """php
      $cluster  = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session  = $cluster->connect("simplex");

      $schema   = $session->schema();
      $keyspace = $schema->keyspace("simplex");

      echo "Name: {$keyspace->name()}" . PHP_EOL;

      /*
       * For Cassandra 2.2+ the replication class name and replication options have
       * been combined into a Map<varchar, varchar> inside the metadata 'replication'
       * instead of two metadata fields 'strategy_class' and 'strategy_options'.
       */
      $replication_options = $keyspace->replicationOptions();
      if ($replication_options instanceof Dse\Map &&
          count($replication_options) > 1) {
          echo "Replication Class: {$replication_options["class"]}" . PHP_EOL;
          echo "  replication_factor:{$replication_options["replication_factor"]}" . PHP_EOL;
      } else {
          echo "Replication Class: {$keyspace->replicationClassName()}" . PHP_EOL;
          foreach ($replication_options as $name => $value) {
              echo "  {$name}:{$value}" . PHP_EOL;
          }
      }

      echo "Has Durable Writes: " . ($keyspace->hasDurableWrites() ? "True" : "False" ). PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Name: simplex
      Replication Class: org.apache.cassandra.locator.SimpleStrategy
        replication_factor:1
      Has Durable Writes: False
      """

    Scenario: Getting table metadata
    Given the following example:
      """php
      $cluster = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");

      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Name: {$table->name()}" . PHP_EOL;
      echo "Bloom Filter: {$table->bloomFilterFPChance()}" . PHP_EOL;
      echo "Comment: {$table->comment()}" . PHP_EOL;

      /*
       * For Cassandra 2.2+ the compaction strategy class name and compaction
       * strategy options have been combined into a Map<varchar, varchar> inside
       * the metadata 'compaction' instead of two metadata fields
       * 'compaction_strategy_class' and 'compaction_strategy_options'.
       */
      $compaction_options = $table->compactionStrategyOptions();
      $compression_parameters = $table->compressionParameters();
      echo "Compaction Class: ";
      if ($compaction_options instanceof Dse\Map &&
          count($compaction_options) > 1) {
          echo $compaction_options["class"] . PHP_EOL;
          echo "  sstable_size_in_mb: {$compaction_options["sstable_size_in_mb"]}" . PHP_EOL;
          echo "  sstable_compression: {$compression_parameters["class"]}" . PHP_EOL;
      } else {
          echo $table->compactionStrategyClassName() . PHP_EOL;
          foreach ($compaction_options as $key => $value) {
              echo "  {$key}: {$value}" . PHP_EOL;
          }
          foreach ($compression_parameters as $key => $value) {
              echo "  {$key}: {$value}" . PHP_EOL;
          }
      }

      echo "Garbage Collection Grace Seconds: {$table->gcGraceSeconds()}" . PHP_EOL;
      echo "Read Repair Chance: {$table->readRepairChance()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Name: values
      Bloom Filter: 0.5
      Comment: Schema Metadata Feature
      Compaction Class: org.apache.cassandra.db.compaction.LeveledCompactionStrategy
        sstable_size_in_mb: 37
        sstable_compression: org.apache.cassandra.io.compress.DeflateCompressor
      Garbage Collection Grace Seconds: 3600
      Read Repair Chance: 0.75
      """

    @cassandra-version-less-2.1
    Scenario: Getting table metadata for io cache and replicate on write
    Given the following schema:
      """cql
      ALTER TABLE simplex.values
        WITH
          populate_io_cache_on_flush='true' AND
          replicate_on_write='false';
      """
    And the following example:
      """php
      $cluster = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");

      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Populate I/O Cache on Flush: " . ($table->populateIOCacheOnFlush() ? "True" : "False") . PHP_EOL;
      echo "Replicate on Write: " . ($table->replicateOnWrite() ? "True" : "False") . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Populate I/O Cache on Flush: True
      Replicate on Write: False
      """

    @cassandra-version-only-2.0
    Scenario: Getting table metadata for index interval
    Given the following schema:
      """cql
      ALTER TABLE simplex.values
        WITH
          index_interval = '512';
      """
    And the following example:
      """php
      $cluster = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Index Interval: {$table->indexInterval()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Index Interval: 512
      """

    @cassandra-version-2.0
    Scenario: Getting table metadata for default TTL, memtable flush period and speculative retry
    Given the following schema:
      """cql
      ALTER TABLE simplex.values
        WITH
          default_time_to_live = '10000' AND
          memtable_flush_period_in_ms = '100' AND
          speculative_retry = '10ms';
      """
    And the following example:
      """php
      $cluster = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Default TTL: {$table->defaultTTL()}" . PHP_EOL;
      echo "Memtable Flush Period: {$table->memtableFlushPeriodMs()}" . PHP_EOL;
      echo "Speculative Retry: " . intval($table->speculativeRetry()) . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Default TTL: 10000
      Memtable Flush Period: 100
      Speculative Retry: 10
      """

    @cassandra-version-2.1
    Scenario: Getting table metadata for max and min index intervals
    Given the following schema:
      """cql
      ALTER TABLE simplex.values
        WITH
          max_index_interval = '16' AND
          min_index_interval = '4';
      """
    And the following example:
      """php
      $cluster = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session = $cluster->connect("simplex");
      $schema  = $session->schema();
      $table   = $schema->keyspace("simplex")->table("values");

      echo "Maximum Index Interval: {$table->maxIndexInterval()}" . PHP_EOL;
      echo "Minimum Index Interval: {$table->minIndexInterval()}" . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      Maximum Index Interval: 16
      Minimum Index Interval: 4
      """

    @cassandra-version-2.0
    Scenario: Getting metadata for column and types
    Given the following example:
      """php
      $cluster   = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session   = $cluster->connect("simplex");
      $schema    = $session->schema();
      $table     = $schema->keyspace("simplex")->table("values");
      foreach ($table->columns() as $column) {
          echo "{$column->name()}: {$column->type()}" . PHP_EOL;
      }
      """
    When it is executed
    Then its output should contain disregarding order:
      """
      ascii_value: ascii
      bigint_value: bigint
      blob_value: blob
      decimal_value: decimal
      double_value: double
      float_value: float
      id: int
      inet_value: inet
      int_value: int
      list_value: list<bigint>
      map_value: map<timestamp, double>
      set_value: set<float>
      timestamp_value: timestamp
      timeuuid_value: timeuuid
      uuid_value: uuid
      varint_value: varint
      """

    @cassandra-version-2.1
    Scenario: Getting metadata for user types
    Given the following schema:
      """php
      CREATE TYPE simplex.type1 (a int, b text);
      CREATE TYPE simplex.type2 (a map<timeuuid, int>, b bigint);
      CREATE TYPE simplex.type3 (a map<blob, frozen<set<varint>>>, b list<uuid>);
      """
    And the following example:
      """php
      $cluster   = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->build();
      $session   = $cluster->connect("simplex");
      $schema    = $session->schema();
      $keyspace  = $schema->keyspace("simplex");

      foreach ($keyspace->userTypes() as $name => $type) {
          echo "Name: {$name}" . PHP_EOL;
          echo "Type:" . PHP_EOL;
          foreach ($type->types() as $key => $data_type) {
              echo "  {$key} => {$data_type}" . PHP_EOL;
          }
      }
      """
    When it is executed
    Then its output should contain:
      """
      Name: type1
      Type:
        a => int
        b => text
      Name: type2
      Type:
        a => map<timeuuid, int>
        b => bigint
      Name: type3
      Type:
        a => map<blob, set<varint>>
        b => list<uuid>
      """

    Scenario: Disable schema metadata
    Given the following example:
      """php
      $cluster   = Dse::cluster()
                         ->withContactPoints('127.0.0.1')
                         ->withSchemaMetadata(false)
                         ->build();
      $session   = $cluster->connect("simplex");
      $schema    = $session->schema();
      echo count($schema->keyspaces()) . PHP_EOL;
      """
    When it is executed
    Then its output should contain:
      """
      0
      """
