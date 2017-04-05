<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 */

namespace Dse;

/**
 * @requires extension dse
 */
class PointTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A Point may only be constructed with 1 string argument (WKT) or 2 numbers (x,y)
     */
    public function testTooFewArguments()
    {
        new Point();
    }

    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A Point may only be constructed with 1 string argument (WKT) or 2 numbers (x,y)
     */
    public function testTooManyArguments()
    {
        new Point(1, 2, 3);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 1 must be valid WKT for a Point, 7 given
     */
    public function testOneArgWrongType()
    {
        new Point(7);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 1 must be valid WKT for a Point, 'POINT' given
     */
    public function testOneArgBadWkt()
    {
        new Point("POINT");
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 1 must be a long or a double, 'POINT' given
     */
    public function testTwoArgsBadFirst()
    {
        new Point("POINT", 2);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be a long or a double, 'POINT' given
     */
    public function testTwoArgsBadSecond()
    {
        new Point(2, "POINT");
    }

    /**
     * @dataProvider pointStrings
     */
    public function testToString($x, $y, $expected)
    {
        $point = new Point($x, $y);
        $this->assertEquals($expected, $point->__toString());
    }

    public function pointStrings()
    {
        return array(
            array(1, 2, "1,2"),
            array(1.0, 2.0, "1,2"),
            array(1.5, -2.75, "1.5,-2.75")
        );
    }

    /**
     * @dataProvider pointFromWkt
     */
    public function testConstructFromWkt($wkt, $expected)
    {
        $point = new Point($wkt);
        $this->assertEquals($expected, $point->__toString());
    }

    public function pointFromWkt()
    {
        return array(
            array("POINT (1 2)", "1,2"),
            array("POINT (1.0 2.0)", "1,2"),
            array("POINT (1.5 -2.75)", "1.5,-2.75")
        );
    }

    /**
     * @dataProvider pointWkt
     */
    public function testWkt($x, $y, $expected)
    {
        $point = new Point($x, $y);
        $this->assertEquals($expected, $point->wkt());
    }

    public function pointWkt()
    {
        return array(
            array(1, 2, "POINT (1 2)"),
            array(1.0, 2.0, "POINT (1 2)"),
            array(1.5, -2.75, "POINT (1.5 -2.75)")
        );
    }

    public function testAccessors()
    {
        $point = new Point(3.5, 2.5);
        $this->assertEquals(3.5, $point->x());
        $this->assertEquals(2.5, $point->y());
        $this->assertEquals("org.apache.cassandra.db.marshal.PointType", $point->type()->name());
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($x1, $y1, $x2, $y2, $expected)
    {
        $left = new Point($x1, $y1);
        $right = new Point($x2, $y2);

        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $this->spaceship($left, $right));
        $this->assertEquals(-$expected, $this->spaceship($right, $left));
    }

    public function comparisonTable()
    {
        return array(
            array(0, 9, 1, 1, -1),
            array(1, 2, 1, 3, -1),
            array(1, 2, 1, 2, 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, $this->spaceship(new Point(1, 2), new LineString()));
    }

    public function testProperties()
    {
        $point = new Point(3.5, 2.5);
        $props = get_object_vars($point);
        $this->assertEquals(array("x" => 3.5, "y" => 2.5), $props);
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
