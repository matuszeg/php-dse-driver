<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 */

namespace Dse;
use Dse\DateRange\Precision;
use Dse\DateRange\Bound;

/**
 * @requires extension dse
 */
class DateRangeTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A DateRange may only be constructed with a lower and upper bound in the form of Bound objects or <precision, time_ms|DateTime> pairs
     */
    public function testNoArgsError()
    {
        new DateRange();
    }

    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A DateRange may only be constructed with a lower and upper bound in the form of Bound objects or <precision, time_ms|DateTime> pairs
     */
    public function testTooManyArgsError()
    {
        new DateRange(Precision::YEAR, 1, Precision::DAY, 2, Precision::MONTH, 4);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 1 must be a value from the Precision class, -1 given
     */
    public function testInvalidPrecisionArg1()
    {
        new DateRange(-1, 0);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 3 must be a value from the Precision class, -1 given
     */
    public function testInvalidPrecisionArg3()
    {
        new DateRange(Precision::YEAR, 0, -1, 0);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be a long, a double, a numeric string, a DateTime, or a Dse\Bigint, 1 given
     */
    public function testTimeMsArgWrongTypeArg2()
    {
        new DateRange(Precision::YEAR, true);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2: invalid integer value: 'ab'
     */
    public function testStringArgParseErrorArg2()
    {
        new DateRange(Precision::YEAR, "ab");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage Argument 2 must be between -9223372036854775808 and 9223372036854775807, 9223372036854775808 given
     */
    public function testString64BitArgOverflowErrorArg2()
    {
        new DateRange(Precision::YEAR, "9223372036854775808");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage Argument 2 must be between -9223372036854775808 and 9223372036854775807, -9223372036854775809 given
     */
    public function testString64BitArgUnderflowErrorArg2()
    {
        new DateRange(Precision::YEAR, "-9223372036854775809");
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be between -9223372036854775808 and 9223372036854775807, 1.84467e+19 given
     */
    public function testDouble64BitArgOverflowErrorArg2()
    {
        new DateRange(Precision::YEAR, pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be between -9223372036854775808 and 9223372036854775807, -1.84467e+19 given
     */
    public function testDouble64BitArgUnderflowErrorArg2()
    {
        new DateRange(Precision::YEAR, -pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 4 must be a long, a double, a numeric string, a DateTime, or a Dse\Bigint, 1 given
     */
    public function testTimeMsArgWrongTypeArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, true);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 4: invalid integer value: 'ab'
     */
    public function testStringArgParseErrorArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, "ab");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage Argument 4 must be between -9223372036854775808 and 9223372036854775807, 9223372036854775808 given
     */
    public function testString64BitArgOverflowErrorArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, "9223372036854775808");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage Argument 4 must be between -9223372036854775808 and 9223372036854775807, -9223372036854775809 given
     */
    public function testString64BitArgUnderflowErrorArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, "-9223372036854775809");
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 4 must be between -9223372036854775808 and 9223372036854775807, 1.84467e+19 given
     */
    public function testDouble64BitArgOverflowErrorArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 4 must be between -9223372036854775808 and 9223372036854775807, -1.84467e+19 given
     */
    public function testDouble64BitArgUnderflowErrorArg4()
    {
        new DateRange(Precision::YEAR, 0, Precision::YEAR, -pow(2, 64));
    }

    /**
     * @dataProvider constructTable
     */
    public function testConstruct($dr, $expectedLowerBound, $expectedUpperBound, $expectedIsSingleDate)
    {
        $this->assertEquals($expectedLowerBound, $dr->lowerBound());
        $this->assertEquals($expectedUpperBound, $dr->upperBound());
        $this->assertEquals($expectedIsSingleDate, $dr->isSingleDate());
    }

    public function constructTable()
    {
        return array(
            // Single date DateRange's
            array(new DateRange(Precision::YEAR, 2), new Bound(Precision::YEAR, 2), null, true),
            array(new DateRange(Precision::YEAR, new \DateTime("1970-01-02 00:00:00Z")),
                new Bound(Precision::YEAR, 86400000), null, true),
            array(new DateRange(new Bound(Precision::YEAR, 2)),
                new Bound(Precision::YEAR, 2), null, true),
            array(new DateRange(Bound::unbounded()),
                Bound::unbounded(), null, true),

            // Try out all valid datatypes for timeMs
            array(new DateRange(Precision::YEAR, "2"), new Bound(Precision::YEAR, 2), null, true),
            array(new DateRange(Precision::YEAR, new \Dse\Bigint("2")), new Bound(Precision::YEAR, 2), null, true),
            array(new DateRange(Precision::YEAR, 2.0), new Bound(Precision::YEAR, 2), null, true),
            array(new DateRange(Precision::YEAR, new \Dse\Bigint("9223372036854775807")), new Bound(Precision::YEAR, new \Dse\Bigint("9223372036854775807")), null, true),
            array(new DateRange(Precision::YEAR, new \Dse\Bigint("-9223372036854775808")), new Bound(Precision::YEAR, new \Dse\Bigint("-9223372036854775808")), null, true),

            // Full on Ranges
            array(new DateRange(Precision::YEAR, 0, Precision::YEAR, 2),
                new Bound(Precision::YEAR, 0), new Bound(Precision::YEAR, 2), false),
            array(new DateRange(Precision::YEAR, 0, Precision::YEAR, new \DateTime("1970-01-02 00:00:00Z")),
                new Bound(Precision::YEAR, 0), new Bound(Precision::YEAR, 86400000), false),
            array(new DateRange(Precision::YEAR, 0, new Bound(Precision::YEAR, 2)),
                new Bound(Precision::YEAR, 0), new Bound(Precision::YEAR, 2), false),
            array(new DateRange(Precision::YEAR, 0, Bound::unbounded()),
                new Bound(Precision::YEAR, 0), Bound::unbounded(), false),

            array(new DateRange(Bound::unbounded(), Bound::unbounded()),
                Bound::unbounded(), Bound::unbounded(), false)
        );
    }

    public function testToString()
    {
        $b = new Bound(Precision::MONTH, new \DateTime("1970-01-02 00:00:00Z"));
        $this->assertEquals("*", (new DateRange(Bound::unbounded()))->__toString());
        $this->assertEquals("1970-01-02 00:00:00.000(MONTH)",
            (new DateRange($b))->__toString());
        $this->assertEquals("* TO *", (new DateRange(Bound::unbounded(), Bound::unbounded()))->__toString());
        $this->assertEquals("1970-01-02 00:00:00.000(MONTH) TO *", (new DateRange($b, Bound::unbounded()))->__toString());
        $this->assertEquals("* TO 1970-01-02 00:00:00.000(MONTH)", (new DateRange(Bound::unbounded(), $b))->__toString());
        $this->assertEquals("1970-01-02 00:00:00.000(YEAR) TO 1970-01-02 00:00:00.000(MONTH)", (new DateRange(Precision::YEAR, $b->timeMs(), $b))->__toString());
    }

    public function testType()
    {
        $dr = new DateRange(Bound::unbounded());
        $this->assertEquals("org.apache.cassandra.db.marshal.DateRangeType", $dr->type()->name());
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($left, $right, $expected)
    {
        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $this->spaceship($left, $right));
        $this->assertEquals(-$expected, $this->spaceship($right, $left));
    }

    public function comparisonTable()
    {
        $lowBound = new Bound(Precision::YEAR, 0);
        $midBound = new Bound(Precision::MILLISECOND, 50);
        $highBound = new Bound(Precision::MILLISECOND, 99);

        return array(
            array(new DateRange($lowBound, $highBound), new DateRange($highBound, $lowBound), -1),
            array(new DateRange($lowBound, $midBound), new DateRange($lowBound, $highBound), -1),
            array(new DateRange($lowBound), new DateRange($lowBound, $highBound), -1),
            array(new DateRange($lowBound), new DateRange($highBound), -1),
            array(new DateRange($lowBound), new DateRange($lowBound), 0),
            array(new DateRange($lowBound, $highBound), new DateRange($lowBound, $highBound), 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, $this->spaceship(new DateRange(Bound::unbounded()), new LineString()));
    }

    /**
     * @dataProvider constructTable
     */
    public function testProperties($dr, $expectedLowerBound, $expectedUpperBound, $expectedIsSingleDate)
    {
        $props = get_object_vars($dr);
        $this->assertEquals(array("lowerBound" => $expectedLowerBound,
                                  "upperBound" => $expectedUpperBound),
                            $props);
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
