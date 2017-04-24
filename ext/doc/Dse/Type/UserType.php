<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Type;

/**
 * A class that represents a user type. The user type is able to represent a
 * composite type of one or more types accessed by name.
 */
final class UserType extends Type {

    private function __construct() { }

    /**
     * Associate the user type with a name.
     *
     * @param string $name Name of the user type.
     *
     * @return null Nothing.
     */
    public function withName($name) { }

    /**
     * Returns type name for the user type
     *
     * @return string Name of this type
     */
    public function name() { }

    /**
     * Associate the user type with a keyspace.
     *
     * @param string $keyspace Keyspace that contains the user type.
     *
     * @return null Nothing.
     */
    public function withKeyspace($keyspace) { }

    /**
     * Returns keyspace for the user type
     *
     * @return string
     */
    public function keyspace() { }

    /**
     * Returns type representation in CQL, e.g. keyspace1.type_name1 or
     * `userType<name1:varchar, name2:int>`.
     *
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Returns types of values
     *
     * @return array An array of types
     */
    public function types() { }

    /**
     * Creates a new UserTypeValue from the given name/value pairs. When
     * no values given, creates an empty user type.
     *
     * @param mixed $value ,...      One or more name/value pairs to be added to the user type.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different types than what the
     *                                            user type expects.
     *
     * @return UserTypeValue A user type value with given name/value pairs.
     */
    public function create($value) { }

}
