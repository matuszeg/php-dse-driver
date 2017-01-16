<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * Cluster object is used to create Sessions.
 */
abstract class Type {

    /**
     * Returns the name of this type as string.
     * @return string Name of this type
     */
    public abstract function name();

    /**
     * Returns string representation of this type.
     * @return string String representation of this type
     */
    public abstract function __toString();

    /**
     * Instantiate a value of this type from provided value(s).
     *
     * @throws Exception\InvalidArgumentException when values given cannot be
     *                                            represented by this type.
     * @param mixed $values ,... one or more values to coerce into this type
     * @return mixed a value of this type
     */
    public abstract function create($values);

    /**
     * Get representation of ascii type
     * @return Dse\Type ascii type
     */
    public static final function ascii() { }

    /**
     * Get representation of bigint type
     * @return Dse\Type bigint type
     */
    public static final function bigint() { }

    /**
     * Get representation of smallint type
     * @return Dse\Type smallint type
     */
    public static final function smallint() { }

    /**
     * get representation of tinyint type
     * @return Dse\Type tinyint type
     */
    public static final function tinyint() { }

    /**
     * Get representation of blob type
     * @return Dse\Type blob type
     */
    public static final function blob() { }

    /**
     * @return mixed
     */
    public static final function boolean() { }

    /**
     * Get representation of counter type
     * @return Dse\Type counter type
     */
    public static final function counter() { }

    /**
     * Get representation of decimal type
     * @return Dse\Type decimal type
     */
    public static final function decimal() { }

    /**
     * Get representation of double type
     * @return Dse\Type double type
     */
    public static final function double() { }

    /**
     * Get representation of float type
     * @return Dse\Type float type
     */
    public static final function float() { }

    /**
     * Get representation of int type
     * @return Dse\Type int type
     */
    public static final function int() { }

    /**
     * Get representation of text type
     * @return Dse\Type text type
     */
    public static final function text() { }

    /**
     * Get representation of timestamp type
     * @return Dse\Type timestamp type
     */
    public static final function timestamp() { }

    /**
     * get representation of date type
     * @return Dse\Type date type
     */
    public static final function date() { }

    /**
     * get representation of time type
     * @return Dse\Type time type
     */
    public static final function time() { }

    /**
     * Get representation of uuid type
     * @return Dse\Type uuid type
     */
    public static final function uuid() { }

    /**
     * Get representation of varchar type
     * @return Dse\Type varchar type
     */
    public static final function varchar() { }

    /**
     * Get representation of varint type
     * @return Dse\Type varint type
     */
    public static final function varint() { }

    /**
     * Get representation of timeuuid type
     * @return Dse\Type timeuuid type
     */
    public static final function timeuuid() { }

    /**
     * Get representation of inet type
     * @return Dse\Type inet type
     */
    public static final function inet() { }

    /**
     * Initialize a Collection type
     * @code{.php}
     * <?php
     * use Dse\Type;
     *
     * $collection = Type::collection(Type::int())
     *                   ->create(1, 2, 3, 4, 5, 6, 7, 8, 9);
     *
     * var_dump($collection);
     * @endcode
     * @param Dse\Type $type The type of values
     * @return Type The collection type
     */
    public static final function collection($type) { }

    /**
     * Initialize a set type
     * @code{.php}
     * <?php
     * use Dse\Type;
     *
     * $set = Type::set(Type::varchar())
     *            ->create("a", "b", "c", "d", "e", "f", "g", "h", "i", "j");
     *
     * var_dump($set);
     * @endcode
     * @param Dse\Type $type The types of values
     * @return mixed
     */
    public static final function set($type) { }

    /**
     * Initialize a map type
     * @code{.php}
     * <?php
     * use Dse\Type;
     *
     * $map = Type::map(Type::int(), Type::varchar())
     *            ->create(1, "a", 2, "b", 3, "c", 4, "d", 5, "e", 6, "f")
     *
     * var_dump($map);
     * @endcode
     * @param Dse\Type $keyType The type of keys
     * @param Dse\Type $valueType The type of values
     * @return Dse\Type The map type
     */
    public static final function map($keyType, $valueType) { }

    /**
     * Initialize a tuple type
     * @code{.php}
     * <?php
     * use Dse\Type;
     *
     * $tuple = Type::tuple(Type::varchar(), Type::int())
     *            ->create("a", 123);
     *
     * var_dump($tuple);
     * @endcode
     * @param Dse\Type $types A variadic list of types
     * @return Dse\Type The tuple type
     */
    public static final function tuple($types) { }

    /**
     * Initialize a user type
     * @code{.php}
     * <?php
     * use Dse\Type;
     *
     * $userType = Type::userTupe("a", Type::varchar(), "b", Type::int())
     *                 ->create("a", "abc", "b", 123);
     *
     * var_dump($userType);
     * @endcode
     * @param Dse\Type $types A variadic list of name/type pairs
     * @return Dse\Type The user type
     */
    public static final function userType($types) { }

}
