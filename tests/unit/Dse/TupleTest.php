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

namespace Dse;

/**
 * @requires extension dse
 */
class TupleTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'custom type'
     */
    public function testSupportsOnlyDseTypes()
    {
        new Tuple(array('custom type'));
    }

    /**
     * @dataProvider dseTypes
     */
    public function testSupportsAllDseTypes($types)
    {
        new Tuple($types);
    }

    /**
     * @dataProvider dseTypes
     */
    public function testReturnsItsType($types)
    {
        $tuple = new Tuple($types);
        $this->assertEquals(count($types), count($tuple->type()->types()));
        $this->assertEquals($types, $tuple->type()->types());
    }

    public function dseTypes()
    {
        return array(
            array(array(\Dse::TYPE_TEXT)),
            array(array(\Dse::TYPE_ASCII)),
            array(array(\Dse::TYPE_VARCHAR)),
            array(array(\Dse::TYPE_BIGINT)),
            array(array(\Dse::TYPE_BOOLEAN)),
            array(array(\Dse::TYPE_COUNTER)),
            array(array(\Dse::TYPE_DECIMAL)),
            array(array(\Dse::TYPE_DOUBLE)),
            array(array(\Dse::TYPE_FLOAT)),
            array(array(\Dse::TYPE_INT)),
            array(array(\Dse::TYPE_TIMESTAMP)),
            array(array(\Dse::TYPE_UUID)),
            array(array(\Dse::TYPE_VARINT)),
            array(array(\Dse::TYPE_TIMEUUID)),
            array(array(\Dse::TYPE_INET)),
            array(array(\Dse::TYPE_TIMEUUID, \Dse::TYPE_UUID)),
            array(array(\Dse::TYPE_INT, \Dse::TYPE_BIGINT, \Dse::TYPE_VARINT)),
            array(array(\Dse::TYPE_INT, \Dse::TYPE_BIGINT, \Dse::TYPE_VARINT)),
            array(array(\Dse::TYPE_ASCII, \Dse::TYPE_TEXT, \Dse::TYPE_VARCHAR)),
        );
    }

    /**
     * @dataProvider compositeTypes
     */
    public function testCompositeKeys($type, $value)
    {
        $tuple = Type::tuple($type)->create($value);
        $this->assertEquals($tuple->get(0), $value);
    }

    public function compositeTypes()
    {
        $map_type = Type::map(Type::varchar(), Type::varchar());
        $set_type = Type::set(Type::varchar());
        $list_type = Type::collection(Type::varchar());
        $tuple_type = Type::tuple(Type::varchar(), Type::int());
        return array(
            array($map_type, $map_type->create("a", "1", "b", "2")),
            array($set_type, $set_type->create("a", "b", "c")),
            array($list_type, $list_type->create("a", "b", "c")),
            array($tuple_type, $tuple_type->create("a", 42)),
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  argument must be an instance of Dse\Varint, an instance of Dse\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $tuple = new Tuple(array(\Dse::TYPE_VARINT));
        $tuple->set(0, new Decimal('123'));
    }

    public function testSetAllElements()
    {
        $tuple = new Tuple(array(\Dse::TYPE_BOOLEAN,
            \Dse::TYPE_INT,
            \Dse::TYPE_BIGINT,
            \Dse::TYPE_TEXT,
        ));

        $this->assertEquals(4, $tuple->count());

        $tuple->set(0, true);
        $tuple->set(1, 42);
        $tuple->set(2, new Bigint("123"));
        $tuple->set(3, "abc");

        $this->assertEquals(4, $tuple->count());
        $this->assertEquals($tuple->get(0), true);
        $this->assertEquals($tuple->get(1), 42);
        $this->assertEquals($tuple->get(2), new Bigint("123"));
        $this->assertEquals($tuple->get(3), "abc");
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Index out of bounds
     */
    public function testInvalidSetIndex()
    {
        $tuple = new Tuple(array(\Dse::TYPE_TEXT));
        $tuple->set(1, "invalid index");
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Index out of bounds
     */
    public function testInvalidGetIndex()
    {
        $tuple = new Tuple(array(\Dse::TYPE_TEXT));
        $tuple->set(0, "invalid index");
        $tuple->get(1);
    }

    /**
     * @dataProvider equalTypes
     */
    public function testCompareEquals($value1, $value2)
    {
        $this->assertEquals($value1, $value2);
        $this->assertTrue($value1 == $value2);
    }

    public function equalTypes()
    {
        $setType = Type::set(Type::int());
        return array(
            array(Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(),
                  Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create()),
            array(Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99)),
                  Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99))),
            array(Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a'),
                  Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a'))
        );
    }

    /**
     * @dataProvider notEqualTypes
     */
    public function testCompareNotEquals($value1, $value2)
    {
        $this->assertNotEquals($value1, $value2);
        $this->assertFalse($value1 == $value2);
    }

    public function notEqualTypes()
    {
        $setType = Type::set(Type::int());
        return array(
            array(Type::tuple(Type::int(), Type::varchar(), Type::varint())->create(),
                  Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create()),
            array(Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99)),
                  Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(2, 'b', new Bigint(99))),
            array(Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a'),
                  Type::tuple($setType, Type::varchar())->create($setType->create(4, 5, 6), 'a'))
        );
    }
}
