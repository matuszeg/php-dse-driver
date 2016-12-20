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
     * Get representation of cassandra ascii type
     * @return Cassandra\Type ascii type
     */
    public static final function ascii() { }

    /**
     * Get representation of cassandra bigint type
     * @return Cassandra\Type bigint type
     */
    public static final function bigint() { }

    /**
     * Get representation of cassandra smallint type
     * @return Cassandra\Type smallint type
     */
    public static final function smallint() { }

    /**
     * get representation of cassandra tinyint type
     * @return Cassandra\Type tinyint type
     */
    public static final function tinyint() { }

    /**
     * Get representation of cassandra blob type
     * @return Cassandra\Type blob type
     */
    public static final function blob() { }

    /**
     * Get representation of cassandra boolean type
     * @return Cassandra\Type boolean type
     */
    public static final function boolean() { }

    /**
     * Get representation of cassandra counter type
     * @return Cassandra\Type counter type
     */
    public static final function counter() { }

    /**
     * Get representation of cassandra decimal type
     * @return Cassandra\Type decimal type
     */
    public static final function decimal() { }

    /**
     * Get representation of cassandra double type
     * @return Cassandra\Type double type
     */
    public static final function double() { }

    /**
     * Get representation of cassandra float type
     * @return Cassandra\Type float type
     */
    public static final function float() { }

    /**
     * Get representation of cassandra int type
     * @return Cassandra\Type int type
     */
    public static final function int() { }

    /**
     * Get representation of cassandra text type
     * @return Cassandra\Type text type
     */
    public static final function text() { }

    /**
     * Get representation of cassandra timestamp type
     * @return Cassandra\Type timestamp type
     */
    public static final function timestamp() { }

    /**
     * get representation of cassandra date type
     * @return Cassandra\Type date type
     */
    public static final function date() { }

    /**
     * get representation of cassandra time type
     * @return Cassandra\Type time type
     */
    public static final function time() { }

    /**
     * Get representation of cassandra uuid type
     * @return Cassandra\Type uuid type
     */
    public static final function uuid() { }

    /**
     * Get representation of cassandra varchar type
     * @return Cassandra\Type varchar type
     */
    public static final function varchar() { }

    /**
     * Get representation of cassandra varint type
     * @return Cassandra\Type varint type
     */
    public static final function varint() { }

    /**
     * Get representation of cassandra timeuuid type
     * @return Cassandra\Type timeuuid type
     */
    public static final function timeuuid() { }

    /**
     * Get representation of cassandra inet type
     * @return Cassandra\Type inet type
     */
    public static final function inet() { }

    /**
     * Initialize a Collection type
     * @code{.php}
     * <?php
     * use Cassandra\Type;
     *
     * $collection = Type::collection(Type::int())
     *                   ->create(1, 2, 3, 4, 5, 6, 7, 8, 9);
     *
     * var_dump($collection);
     * @endcode
     * @param Cassandra\Type $type The type of values
     * @return Type The collection type
     */
    public static final function collection($type) { }

    /**
     * Initialize a set type
     * @code{.php}
     * <?php
     * use Cassandra\Type;
     *
     * $set = Type::set(Type::varchar())
     *            ->create("a", "b", "c", "d", "e", "f", "g", "h", "i", "j");
     *
     * var_dump($set);
     * @endcode
     * @param Cassandra\Type $type The types of values
     * @return mixed
     */
    public static final function set($type) { }

    /**
     * Initialize a map type
     * @code{.php}
     * <?php
     * use Cassandra\Type;
     *
     * $map = Type::map(Type::int(), Type::varchar())
     *            ->create(1, "a", 2, "b", 3, "c", 4, "d", 5, "e", 6, "f")
     *
     * var_dump($map);
     * @endcode
     * @param Cassandra\Type $keyType The type of keys
     * @param Cassandra\Type $valueType The type of values
     * @return Cassandra\Type The map type
     */
    public static final function map($keyType, $valueType) { }

    /**
     * Initialize a tuple type
     * @code{.php}
     * <?php
     * use Cassandra\Type;
     *
     * $tuple = Type::tuple(Type::varchar(), Type::int())
     *            ->create("a", 123);
     *
     * var_dump($tuple);
     * @endcode
     * @param Cassandra\Type $types A variadic list of types
     * @return Cassandra\Type The tuple type
     */
    public static final function tuple($types) { }

    /**
     * Initialize a user type
     * @code{.php}
     * <?php
     * use Cassandra\Type;
     *
     * $userType = Type::userTupe("a", Type::varchar(), "b", Type::int())
     *                 ->create("a", "abc", "b", 123);
     *
     * var_dump($userType);
     * @endcode
     * @param Cassandra\Type $types A variadic list of name/type pairs
     * @return Cassandra\Type The user type
     */
    public static final function userType($types) { }

}
