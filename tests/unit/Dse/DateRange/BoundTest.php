<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 */

namespace Dse\DateRange;

/**
 * @requires extension dse
 */
class BoundTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage precision must be a value from the Precision class, -1 given
     */
    public function testInvalidPrecision()
    {
        new Bound(-1, 5);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage timeMs must be a long, a double, a numeric string, a DateTime, or a Dse\Bigint, an instance of Dse\Point given
     */
    public function testInvalidTimeObject()
    {
        new Bound(Precision::YEAR, new \Dse\Point(1, 2));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage timeMs must be a long, a double, a numeric string, a DateTime, or a Dse\Bigint, 1 given
     */
    public function testTimeMsArgWrongType()
    {
        new Bound(Precision::YEAR, true);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage timeMs: invalid integer value: 'ab'
     */
    public function testStringArgParseError()
    {
        new Bound(Precision::YEAR, "ab");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage timeMs must be between -9223372036854775808 and 9223372036854775807, 9223372036854775808 given
     */
    public function testString64BitArgOverflowError()
    {
        new Bound(Precision::YEAR, "9223372036854775808");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage timeMs must be between -9223372036854775808 and 9223372036854775807, -9223372036854775809 given
     */
    public function testString64BitArgUnderflowError()
    {
        new Bound(Precision::YEAR, "-9223372036854775809");
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage timeMs must be between -9223372036854775808 and 9223372036854775807, 1.84467e+19 given
     */
    public function testDouble64BitArgOverflowError()
    {
        new Bound(Precision::YEAR, pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage timeMs must be between -9223372036854775808 and 9223372036854775807, -1.84467e+19 given
     */
    public function testDouble64BitArgUnderflowError()
    {
        new Bound(Precision::YEAR, -pow(2, 64));
    }

    public function testConstructWithDateTime()
    {
        $bound = new Bound(Precision::YEAR, new \DateTime("1970-01-02 00:00:00Z"));
        $this->assertEquals(86400000, $bound->timeMs());
    }

    /**
     * @dataProvider goodAttributes
     */
    public function testAccessors($precision, $timeMs)
    {
        $bound = new Bound($precision, $timeMs);
        $this->assertEquals($precision, $bound->precision());
        $this->assertEquals((string) $timeMs, $bound->timeMs());
    }

    public function goodAttributes()
    {
        return array(
            array(Precision::YEAR, 5),
            array(Precision::YEAR, "5"),
            array(Precision::YEAR, new \Dse\Bigint("5")),
            array(Precision::YEAR, 3.0),
            array(Precision::YEAR, new \Dse\Bigint("9223372036854775807")),
            array(Precision::YEAR, new \Dse\Bigint("-9223372036854775808")),
        );
    }

    public function testUnbounded()
    {
        $bound = Bound::unbounded();
        $this->assertEquals(Precision::UNBOUNDED, $bound->precision());
        $this->assertEquals(-1, $bound->timeMs());
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($precision1, $time1, $precision2, $time2, $expected)
    {
        $left = new Bound($precision1, $time1);
        $right = new Bound($precision2, $time2);

        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $this->spaceship($left, $right));
        $this->assertEquals(-$expected, $this->spaceship($right, $left));
    }

    public function comparisonTable()
    {
        return array(
            array(Precision::DAY, 1, Precision::YEAR, 9, -1),
            array(Precision::YEAR, 2, Precision::DAY, 2, -1),
            array(Precision::DAY, 2, Precision::DAY, 2, 0),

            // All "unbounded" bounds are equal, regardless of timeMs
            array(Precision::UNBOUNDED, 2, Precision::UNBOUNDED, 9, 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, $this->spaceship(new Bound(Precision::YEAR, 2), new \DateTime()));
    }

    public function testProperties()
    {
        $bound = new Bound(Precision::MILLISECOND, 5);
        $props = get_object_vars($bound);
        $this->assertEquals(array("precision" => Precision::MILLISECOND, "timeMs" => 5), $props);
    }

    /**
     * Spaceship (<=>) impl for testing our compare function. The operator exists in PHP7, but not
     * PHP 5, so we implement it ourselves as a function.
     */
    public function spaceship($left, $right) {
        if ($left < $right) {
            return -1;
        } else if ($left == $right) {
            return 0;
        } else {
            return 1;
        }
    }
}
