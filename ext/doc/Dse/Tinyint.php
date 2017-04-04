<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `tinyint` datatype.
 */
final class Tinyint implements Value, Numeric {

    /**
     * Creates a new 8-bit signed integer.
     *
     * @param int|double|string $value The value as an integer, double or string
     */
    public function __construct($value) { }

    /**
     * @return string
     */
    public function __toString() { }

    /**
     * The type of this value (tinyint).
     *
     * @return Type
     */
    public function type() { }

    /**
     * Returns the integer value.
     *
     * @return int integer value
     */
    public function value() { }

    /**
     * @param Numeric $num a number to add to this one
     * @return Numeric sum
     */
    public function add($num) { }

    /**
     * @param Numeric $num a number to subtract from this one
     * @return Numeric difference
     */
    public function sub($num) { }

    /**
     * @param Numeric $num a number to multiply this one by
     * @return Numeric product
     */
    public function mul($num) { }

    /**
     * @param Numeric $num a number to divide this one by
     * @return Numeric quotient
     */
    public function div($num) { }

    /**
     * @param Numeric $num a number to divide this one by
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
     * Minimum possible Tinyint value
     *
     * @return Tinyint minimum value
     */
    public static function min() { }

    /**
     * Maximum possible Tinyint value
     *
     * @return Tinyint maximum value
     */
    public static function max() { }

}
