<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `smallint` datatype.
 */
final class Smallint implements Value, Numeric {

    /**
     * Creates a new 16bit integer.
     * @param string $value integer value as a string
     */
    public function __construct($value) { }

    /**
     * @return mixed
     */
    public function __toString() { }

    /**
     * The type of this value (smallint).
     * @return Type
     */
    public function type() { }

    /**
     * Returns the integer value.
     * @return int integer value
     */
    public function value() { }

    /**
     * @param Dse\Numeric $num a number to add to this one
     * @return Numeric sum
     */
    public function add($num) { }

    /**
     * @param Dse\Numeric $num a number to subtract from this one
     * @return Numeric difference
     */
    public function sub($num) { }

    /**
     * @param Dse\Numeric $num a number to multiply this one by
     * @return Numeric product
     */
    public function mul($num) { }

    /**
     * @param Dse\Numeric $num a number to divide this one by
     * @return Numeric quotient
     */
    public function div($num) { }

    /**
     * @param Dse\Numeric $num a number to divide this one by
     * @return Numeric remainder
     */
    public function mod($num) { }

    /**
     * @return Numeric absolute value
     */
    public function abs() { }

    /**
     * @return Numeric negative value
     */
    public function neg() { }

    /**
     * @return Numeric square root
     */
    public function sqrt() { }

    /**
     * @return int this number as int
     */
    public function toInt() { }

    /**
     * @return float this number as float
     */
    public function toDouble() { }

    /**
     * Minimum possible Smallint value
     * @return Smallint minimum value
     */
    public static function min() { }

    /**
     * Maximum possible Smallint value
     * @return Smallint maximum value
     */
    public static function max() { }

}
