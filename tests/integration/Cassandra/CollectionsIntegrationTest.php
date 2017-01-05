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
 * A base class for collections integration tests
 */
abstract class CollectionsIntegrationTest extends DatatypeIntegrationTests {
    /**
     * Create user types after initializing cluster and session
     */
    public function setUp() {
        parent::setUp();

        foreach ($this->compositeCassandraTypes() as $cassandraType) {
            if ($cassandraType[0] instanceof Cassandra\Type\UserType) {
                $this->createUserType($cassandraType[0]);
            }
        }

        foreach ($this->nestedCassandraTypes() as $cassandraType) {
            if ($cassandraType[0] instanceof Cassandra\Type\UserType) {
                $this->createUserType($cassandraType[0]);
            }
        }
    }

    /**
     * Composite Cassandra types (list, map, set, tuple, and UDT) to be used by
     * data providers
     */
    public function compositeCassandraTypes() {
        $collectionType = Cassandra\Type::collection(Cassandra\Type::varchar());
        $setType = Cassandra\Type::set(Cassandra\Type::varchar());
        $mapType = Cassandra\Type::map(Cassandra\Type::varchar(), Cassandra\Type::int());
        $tupleType = Cassandra\Type::tuple(Cassandra\Type::varchar(), Cassandra\Type::int(), Cassandra\Type::bigint());
        $userType = Cassandra\Type::userType("a", Cassandra\Type::varchar(), "b", Cassandra\Type::int(), "c", Cassandra\Type::bigint());
        $userType = $userType->withName(self::userTypeString($userType));

        return array(
            array($collectionType, array($collectionType->create("a", "b", "c"),
                                         $collectionType->create("d", "e", "f"),
                                         $collectionType->create("x", "y", "z"))),
            array($setType, array($setType->create("a", "b", "c"),
                                  $setType->create("d", "e", "f"),
                                  $setType->create("x", "y", "z"))),
            array($mapType, array($mapType->create("a", 1, "b", 2, "c", 3),
                                  $mapType->create("d", 4, "e", 5, "f", 6),
                                  $mapType->create("x", 7, "y", 8, "z", 9))),
            array($tupleType, array($tupleType->create("a", 1, new Cassandra\Bigint(2)),
                                    $tupleType->create("b", 3, new Cassandra\Bigint(4)),
                                    $tupleType->create("c", 5, new Cassandra\Bigint(6)))),
            array($userType, array($userType->create("a", "x", "b", 1, "c", new Cassandra\Bigint(2)),
                                   $userType->create("a", "y", "b", 3, "c", new Cassandra\Bigint(4)),
                                   $userType->create("a", "z", "b", 5, "c", new Cassandra\Bigint(6))))
        );
    }

    /**
     * Nested composite Cassandra types (list, map, set, tuple, and UDT) to be
     * used by data providers
     */
    public function nestedCassandraTypes() {
        $compositeCassandraTypes = $this->compositeCassandraTypes();

        foreach ($compositeCassandraTypes as $nestedType) {
            $type = Cassandra\Type::collection($nestedType[0]);
            $nestedCassandraTypes[] = array($type, array($type->create($nestedType[1][0])));
        }

        foreach ($compositeCassandraTypes as $nestedType) {
            $type = Cassandra\Type::set($nestedType[0]);
            $nestedCassandraTypes[] = array($type, array($type->create($nestedType[1][0])));
        }

        foreach ($compositeCassandraTypes as $nestedType) {
            $type = Cassandra\Type::map($nestedType[0], $nestedType[0]);
            $nestedCassandraTypes[] = array($type, array($type->create($nestedType[1][0], $nestedType[1][1])));
        }

        foreach ($compositeCassandraTypes as $nestedType) {
            $type = Cassandra\Type::tuple($nestedType[0], $nestedType[0]);
            $nestedCassandraTypes[] = array($type, array($type->create($nestedType[1][0], $nestedType[1][1])));
        }

        foreach ($compositeCassandraTypes as $nestedType) {
            $type = Cassandra\Type::userType("a", $nestedType[0], "b", $nestedType[0]);
            $type = $type->withName(self::userTypeString($type));
            $nestedCassandraTypes[] = array($type, array($type->create("a", $nestedType[1][0], "b", $nestedType[1][1])));
        }

        return $nestedCassandraTypes;
    }
}
