<?php

/**
 * Copyright (c) 2017 DataStax, Inc.
 *
 * This software can be used solely with DataStax Enterprise. Please consult the
 * license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
 */

/**
 * DSE data type integration tests.
 */
class DseDataTypeTest extends DseIntegrationTest {
    /**
     * Table format for the data types (create cql query)
     */
    const TABLE_FORMAT = "CREATE TABLE %s.%s (id timeuuid PRIMARY KEY, value %s)";
    /**
     * Table format for the data types used as primary key (create cql query)
     */
    const TABLE_PRIMARY_KEY_FORMAT = "CREATE TABLE %s.%s (id %s PRIMARY KEY, value timeuuid)";
    /**
     * Insert format for the data types (insert cql query)
     */
    const INSERT_FORMAT = "INSERT INTO %s.%s (id, value) VALUES(?, ?)";
    /**
     * @var \Cassandra\Timeuuid ID to use for data type under test
     */
    private $id;

    /**
     * @inheritdoc
     */
    public function setUp() {
        // Indicate data provider is in use
        $this->using_data_provider = true;

        // Call the parent function
        parent::setUp();

        // Create the timeuuid for the test
        $this->id = new Dse\Timeuuid();

        // Create the user types
        foreach ($this->data_types() as $composite_data_type) {
            if ($composite_data_type[0] instanceof Dse\Type\UserType) {
                $this->create_user_data_type($composite_data_type[0]);
            }
        }
    }

    /**
     * Create and prepare a statement for inserting the data type
     *
     * @return Dse\PreparedStatement Prepared statement for insert
     */
    private function create_and_prepare_statement() {
        $query = sprintf(self::INSERT_FORMAT, $this->keyspace, $this->table);
        return $this->session->prepare($query);
    }

    /**
     * Create a simple statement for inserting the data type
     *
     * @return Dse\SimpleStatement Simple statement for insert
     */
    private function create_simple_statement() {
        $query = sprintf(self::INSERT_FORMAT, $this->keyspace, $this->table);
        return new Dse\SimpleStatement($query);
    }

    /**
     * Create a table for the given CQL type
     *
     * @param string $cql_type CQL type under test
     * @param bool|true $primary_key (Optional) True if $cql_type should be
     *                                          used as the primary key; false
     *                                          otherwise (default: false)
     */
    private function create_table($cql_type, $primary_key = false) {
        // Determine if the normal or primary key table is being created
        if ($primary_key) {
            $query = sprintf(self::TABLE_PRIMARY_KEY_FORMAT,
                $this->keyspace, $this->table, $cql_type);
        } else {
            $query = sprintf(self::TABLE_FORMAT,
                $this->keyspace, $this->table, $cql_type);
        }

        // Create the table
        $statement = new Dse\SimpleStatement($query);
        if (self::$configuration->verbose) {
            echo "Executing Query: {$query}" . PHP_EOL;
        }
        $this->session->execute($statement);
    }

    /**
     * Insert a row using the given statement
     *
     * @param mixed $id ID/Primary key to insert
     * @param mixed $value Value to associate with primary key
     * @param Dse\Statement|null (Optional) Statement to execute
     */
    private function insert($id, $value, $statement = null) {
        // Create the execution options for the statement execution
        $options = array(
            "arguments" => array(
                "id" => $id,
                "value" => $value
            )
        );

        // Determine if the statement should be created
        if (is_null($statement)) {
            $statement = $this->create_simple_statement();
        }

        // Execute the insert statement
        $this->session->execute($statement, $options);
    }

    /**
     * Insert a row using a batch statement for a prepared or simple statement
     *
     * @param mixed $id ID/Primary key to insert
     * @param mixed $value Value to associate with primary key
     * @param bool $simple_statement (Optional) True if simple statement should
     *                                          be used; false prepared
     *                                          statement is created and used
     */
    private function insert_batch($id, $value, $simple_statement = true) {
        // Create the batch statement and add the arguments
        if ($simple_statement) {
            $statement = $this->create_simple_statement();
            $batch = new Dse\BatchStatement(Dse::BATCH_LOGGED);
            $batch->add($statement, array(
                $id,
                $value
            ));
        } else {
            $statement = $this->create_and_prepare_statement();
            $batch = new Dse\BatchStatement(Dse::BATCH_LOGGED);
            $batch->add($statement, array(
                "id" => $id,
                "value" => $value
            ));
        }

        // Execute the insert batch statement
        $this->session->execute($batch);
    }

    /**
     * Validate the rows and assert that only one row exists and the expected
     * id and value are equal to the returned values from the server.
     *
     * @param Dse\Rows $rows Rows to validate
     * @param mixed $expected_id Expected id
     * @param mixed $expected_value Expected value
     */
    private function validate($rows, $expected_id, $expected_value) {
        $this->assertCount(1, $rows);
        $result = $rows[0];
        $this->assertEquals($expected_id, $result["id"]);
        $this->assertEquals($expected_value, $result["value"]);
    }

    /**
     * Perform insert using a simple statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster using simple
     * statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:basic, queries:geospatial
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider data_type_provider
     * @test
     */
    public function testSimpleStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type);
        $this->insert($this->id, $value);

        // Validate the data type
        $this->validate($this->select_all_rows(), $this->id, $value);
    }

    /**
     * Perform insert as a primary key using a simple statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster where the value
     * inserted is used as the primary key using simple statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:basic, queries:geospatial
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider primary_key_data_type_provider
     * @test
     */
    public function testPrimaryKeySimpleStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type, true);
        $this->insert($value, $this->id);

        // Validate the data type
        $this->validate($this->select_all_rows(), $value, $this->id);
    }

    /**
     * Perform insert using a prepared statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster using prepared
     * statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:basic, queries:geospatial, prepared_statements
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider data_type_provider
     * @test
     */
    public function testPreparedStatement($type, $cql_type, $value) {
        // Create the table and prepare the statement for the data type
        $this->create_table($cql_type);
        $statement = $this->create_and_prepare_statement();

        // Insert the data type
        $this->insert($this->id, $value, $statement);

        // Validate the data type
        $this->validate($this->select_all_rows(), $this->id, $value);
    }

    /**
     * Perform insert as a primary key using a prepared statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster where the value
     * inserted is used as the primary key using prepared statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:basic, queries:geospatial, prepared_statements
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider primary_key_data_type_provider
     * @test
     */
    public function testPrimaryKeyPreparedStatement($type, $cql_type, $value) {
        // Create the table and prepare the statement for the data type
        $this->create_table($cql_type, true);
        $statement = $this->create_and_prepare_statement();

        // Insert the data type
        $this->insert($value, $this->id, $statement);

        // Validate the data type
        $this->validate($this->select_all_rows(), $value, $this->id);
    }

    /**
     * Perform insert using a batch statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster using batch statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:batch, queries:geospatial
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider data_type_provider
     * @test
     */
    public function testBatchStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type);
        $this->insert_batch($this->id, $value);

        // Validate the data type
        $this->validate($this->select_all_rows(), $this->id, $value);
    }

    /**
     * Perform insert using a batch statement operation with prepared statement
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster using batch statements.
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:batch, queries:geospatial, prepared_statements
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider data_type_provider
     * @test
     */
    public function testBatchStatementWithPreparedStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type);
        $this->insert_batch($this->id, $value, false);

        // Validate the data type
        $this->validate($this->select_all_rows(), $this->id, $value);
    }

    /**
     * Perform insert as a primary key using a batch statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster where the value
     * inserted is used as the primary key using batch statements.
     *
     * NOTE: Only prepared statements will be added to the batch statement in
     *       order to simplify the test and allow for named parameter arguments
     *       when adding to the batch statement
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:batch, queries:geospatial
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider primary_key_data_type_provider
     * @test
     */
    public function testPrimaryKeyBatchStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type, true);
        $this->insert_batch($value, $this->id);

        // Validate the data type
        $this->validate($this->select_all_rows(), $value, $this->id);
    }

    /**
     * Perform insert as a primary key using a batch statement operation
     *
     * This test will perform multiple inserts and select operations using the
     * data type provider against a single node cluster where the value
     * inserted is used as the primary key using batch statements.
     *
     * NOTE: Only prepared statements will be added to the batch statement in
     *       order to simplify the test and allow for named parameter arguments
     *       when adding to the batch statement
     *
     * @jira_ticket PHP-103, PHP-104
     * @test_category queries:batch, queries:geospatial
     * @since 1.0.0
     * @expected_result Data type values are inserted and validated
     *
     * @param mixed $type Data type under test (not used)
     * @param string $cql_type CQL data type under test
     * @param mixed $value Value under test
     *
     * @dataProvider primary_key_data_type_provider
     * @test
     */
    public function testPrimaryKeyBatchStatementWithPreparedStatement($type, $cql_type, $value) {
        // Create the table and insert the data type
        $this->create_table($cql_type, true);
        $this->insert_batch($value, $this->id, false);

        // Validate the data type
        $this->validate($this->select_all_rows(), $value, $this->id);
    }
}