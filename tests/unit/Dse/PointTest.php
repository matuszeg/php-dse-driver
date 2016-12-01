<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 */

namespace Dse;

/**
 * @requires extension dse
 */
class PointTest extends \PHPUnit_Framework_TestCase
{
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
        // The following assert is commented out because it causes the test suite to eventually
        // complete with this:
        //   zend_mm_heap corrupted
        //   Segmentation fault: 11
//        $this->assertEquals("org.apache.cassandra.db.marshal.PointType", $point->type()->name());
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($x1, $y1, $x2, $y2, $expected)
    {
        $this->assertEquals($expected, new Point($x1, $y1) <=> new Point($x2, $y2));
        $this->assertEquals(-$expected, new Point($x2, $y2) <=> new Point($x1, $y1));
    }

    public function comparisonTable()
    {
        return array(
            array(1, 2, 1, 2, 0),
            array(1, 2, 1, 3, -1),
            array(1, 2, 1, 1, 1),
            array(2, 1, 1, 2, 1),
            array(2, 3, 1, 2, 1),
        );
    }

    public function testProperties()
    {
        $point = new Point(3.5, 2.5);
        $props = get_object_vars($point);
        $this->assertEquals(3.5, $props["x"]);
        $this->assertEquals(2.5, $props["y"]);
        $this->assertEquals(2, sizeof($props));
    }
}
