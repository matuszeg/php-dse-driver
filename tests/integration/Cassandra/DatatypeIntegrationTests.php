<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Create an alias for DSE extension to share core test framework
use \Dse as Cassandra;

/**
 * A base class for data type integration tests
 */
abstract class DatatypeIntegrationTests extends IntegrationTest {
    /**
     * @inheritdoc
     */
    public function setUp() {
        // Indicate data provider is in use
        $this->using_data_provider = true;

        // Call the parent function
        parent::setUp();
    }

    /**
     * Scalar Cassandra types to be used by data providers
     */
    public function scalarCassandraTypes() {
        return array(
            array(Cassandra\Type::ascii(), array("a", "b", "c")),
            array(Cassandra\Type::bigint(), array(new Cassandra\Bigint("1"), new Cassandra\Bigint("2"), new Cassandra\Bigint("3"))),
            array(Cassandra\Type::blob(), array(new Cassandra\Blob("x"), new Cassandra\Blob("y"), new Cassandra\Blob("z"))),
            array(Cassandra\Type::boolean(), array(true, false, true, false)),
            array(Cassandra\Type::date(), array(new Cassandra\Date(), new Cassandra\Date(0), new Cassandra\Date(-86400), new Cassandra\Date(86400))),
            array(Cassandra\Type::decimal(), array(new Cassandra\Decimal(1.1), new Cassandra\Decimal(2.2), new Cassandra\Decimal(3.3))),
            array(Cassandra\Type::double(), array(1.1, 2.2, 3.3, 4.4)),
            array(Cassandra\Type::float(), array(new Cassandra\Float(1.0), new Cassandra\Float(2.2), new Cassandra\Float(2.2))),
            array(Cassandra\Type::inet(), array(new Cassandra\Inet("127.0.0.1"), new Cassandra\Inet("127.0.0.2"), new Cassandra\Inet("127.0.0.3"))),
            array(Cassandra\Type::smallint(), array(Cassandra\Smallint::min(), Cassandra\Smallint::max(), new Cassandra\Smallint(0), new Cassandra\Smallint(74))),
            array(Cassandra\Type::text(), array("a", "b", "c", "x", "y", "z")),
            array(Cassandra\Type::time(), array(new Cassandra\Time(), new Cassandra\Time(0), new Cassandra\Time(1234567890))),
            array(Cassandra\Type::tinyint(), array(Cassandra\Tinyint::min(), Cassandra\Tinyint::max(), new Cassandra\Tinyint(0), new Cassandra\Tinyint(37))),
            array(Cassandra\Type::timestamp(), array(new Cassandra\Timestamp(123), new Cassandra\Timestamp(456), new Cassandra\Timestamp(789))),
            array(Cassandra\Type::timeuuid(), array(new Cassandra\Timeuuid(0), new Cassandra\Timeuuid(1), new Cassandra\Timeuuid(2))),
            array(Cassandra\Type::uuid(),  array(new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"),
                                                 new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c3"),
                                                 new Cassandra\Uuid("03398c99-c635-4fad-b30a-3b2c49f785c4"))),
            array(Cassandra\Type::varchar(), array("a", "b", "c", "x", "y", "z")),
            array(Cassandra\Type::varint(), array(new Cassandra\Varint(1), new Cassandra\Varint(2), new Cassandra\Varint(3))),
        );
    }

    /**
     * Create a table using $type for the value's type and insert $value using
     * positional parameters.
     *
     * @param $type Cassandra\Type
     * @param $value mixed
     */
    public function createTableInsertAndVerifyValueByIndex($type, $value) {
        $key = "key";
        $options = array('arguments' => array($key, $value));
        $this->createTableInsertAndVerifyValue($type, $options, $key, $value);
    }

    /**
     * Create a table using $type for the value's type and insert $value using
     * named parameters.
     *
     * @param $type Cassandra\Type
     * @param $value mixed
     */
    public function createTableInsertAndVerifyValueByName($type, $value) {
        $key = "key";
        $options = array('arguments' => array("key" => $key, "value" => $value));
        $this->createTableInsertAndVerifyValue($type, $options, $key, $value);
    }

    /**
     * Create a user type in the current keyspace
     *
     * @param $userType Cassandra\Type\UserType
     */
    public function createUserType($userType) {
        $query  = "CREATE TYPE IF NOT EXISTS %s.%s (%s)";
        $fieldsString = implode(", ", array_map(function ($name, $type) {
            return "$name " . self::typeString($type);
        }, array_keys($userType->types()), $userType->types()));
        $query = sprintf($query, $this->keyspace, $this->userTypeString($userType), $fieldsString);
        $this->session->execute(new Cassandra\SimpleStatement($query));
    }

    /**
     * Create a table named for the CQL $type parameter
     *
     * @param $type Cassandra\Type
     * @return string Table name generated from $type
     */
    public function createTable($type) {
        $query = "CREATE TABLE IF NOT EXISTS %s.%s (key text PRIMARY KEY, value %s)";

        $cqlType = $this->typeString($type);
        $tableName = "table_" . str_replace(array("-"), "", (string)(new Cassandra\Uuid()));

        $query = sprintf($query, $this->keyspace, $tableName, $cqlType);

        $this->session->execute(new Cassandra\SimpleStatement($query));

        return $tableName;
    }

    /**
     * Create a new table with specified type and insert and verify value
     *
     * @param $type Cassandra\Type
     * @param $options Cassandra\ExecutionOptions
     * @param $key string
     * @param $value mixed
     */
    protected function createTableInsertAndVerifyValue($type, $options, $key, $value) {
        $tableName = $this->createTable($type);

        $this->insertValue($tableName, $options);

        $this->verifyValue($tableName, $type, $key, $value);
    }

    /**
     * Insert a value into table
     *
     * @param $tableName string
     * @param $options Cassandra\ExecutionOptions
     */
    protected function insertValue($tableName, $options) {
        $insertQuery = "INSERT INTO $this->keyspace.$tableName (key, value) VALUES (?, ?)";

        $this->session->execute(new Cassandra\SimpleStatement($insertQuery), $options);
    }

    /**
     * Verify value
     *
     * @param $tableName string
     * @param $type Cassandra\Type
     * @param $key string
     * @param $value mixed
     */
    protected function verifyValue($tableName, $type, $key, $value) {
        $selectQuery = "SELECT * FROM $this->keyspace.$tableName WHERE key = ?";

        $options = array('arguments' => array($key));

        $result = $this->session->execute(new Cassandra\SimpleStatement($selectQuery), $options);

        $this->assertEquals(count($result), 1);

        $row = $result->first();

        $this->assertEquals($row['value'], $value);
        $this->assertTrue($row['value'] == $value);
        if (isset($row['value'])) {
            $this->assertEquals(count($row['value']), count($value));
            if (is_object($row['value'])) {
                $this->assertEquals($row['value']->type(), $type);
            }
        }
    }

    /**
     * Generate a type string suitable for creating a new table or user type
     * using CQL
     *
     * @param $type Cassandra\Type
     * @return string String representation of type
     */
    public static function typeString($type) {
        if ($type instanceof Cassandra\Type\Tuple || $type instanceof Cassandra\Type\Collection ||
            $type instanceof Cassandra\Type\Map || $type instanceof Cassandra\Type\Set ||
            $type instanceof Cassandra\Type\UserType) {
            return sprintf("frozen<%s>", $type);
        } else {
            return (string)$type;
        }
    }

    /**
     * Generate a user type name string suitable for creating a new table or
     * user type using CQL
     *
     * @param $userType Cassandra\Type
     * @return string String representation of the UserType
     */
    public static function userTypeString($userType) {
        return sprintf("%s", implode("_", array_map(function ($name, $type) {
            return $name . str_replace(array("frozen", "<", " ", ",", ">"), "", $type);
        }, array_keys($userType->types()), $userType->types())));
    }
}

