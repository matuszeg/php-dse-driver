<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of the CQL `decimal` datatype
 *
 * The actual value of a decimal is `$value * pow(10, $scale * -1)`
 */
final class Decimal implements Value, Numeric {

    /**
     * Creates a decimal from a given decimal string:
     *
     * ~~~{.php}
     * <?php
     * $decimal = new Dse::Decimal("1313123123.234234234234234234123");
     *
     * $this->assertEquals(21, $decimal->scale());
     * $this->assertEquals("1313123123234234234234234234123", $decimal->value());
     * ~~~
     * @param string $value Any decimal string
     */
    public function __construct($value) { }

    /**
     * String representation of this decimal.
     * @return string Decimal value
     */
    public function __toString() { }

    /**
     * The type of this decimal.
     * @return Type
     */
    public function type() { }

    /**
     * Numeric value of this decimal as string.
     * @return string Numeric value
     */
    public function value() { }

    /**
     * Scale of this decimal as int.
     * @return int Scale
     */
    public function scale() { }

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

}
