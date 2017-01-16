<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 * A class that represents the map type. The map type contains two types that
 * represents the types of the key and value contained in the map.
 */
final class Map extends Type {

    private function __construct() { }

    /**
     * Returns "map"
     * @return string "map"
     */
    public function name() { }

    /**
     * Returns type of keys
     * @return Type Type of keys
     */
    public function keyType() { }

    /**
     * Returns type of values
     * @return Type Type of values
     */
    public function valueType() { }

    /**
     * Returns type representation in CQL, e.g. `map<varchar, int>`
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Creates a new Dse\Map from the given values.
     *
     * @code{.php}
     * <?php
     * use Dse\Type;
     * use Dse\Uuid;
     *
     * $type = Type::map(Type::uuid(), Type::varchar());
     * $map = $type->create(new Uuid(), 'first uuid',
     *                      new Uuid(), 'second uuid',
     *                      new Uuid(), 'third uuid');
     *
     * var_dump($map);
     * @endcode
     *
     * @throws Exception\InvalidArgumentException when keys or values given are
     *                                            of a different type than what
     *                                            this map type expects.
     *
     *                          is a key and each even value is a value for the
     *                          map, e.g. `create(key, value, key, value)`.
     *                          When no values given, creates an empty map.
     * @param mixed $value ,... An even number of values, where each odd value
     * @return Dse\Map A set with given values.
     */
    public function create($value) { }

}
