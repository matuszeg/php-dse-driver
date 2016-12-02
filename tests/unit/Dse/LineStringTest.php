<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 */

namespace Dse;

/**
 * @requires extension dse
 */
class LineStringTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A line-string must have at least two points or be empty
     */
    public function testWrongNumberOfArguments()
    {
        new LineString(new Point(2, 3));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be an instance of Dse\Point, 7 given
     */
    public function testWrongTypeOfArguments()
    {
        new LineString(new Point(2, 3), 7);
    }

    /**
     * @dataProvider lineStrings
     */
    public function testToString($pointSpecs, $expected)
    {
        $lineString = $this->makeLineString($pointSpecs);
        $this->assertEquals($expected, $lineString->__toString());
    }

    public function lineStrings()
    {
        return array(
            array(array(2, 3, 3, 4), "2,3 to 3,4"),
            array(array(), "")
        );
    }

    /**
     * @dataProvider lineWkt
     */
    public function testWkt($pointSpecs, $expected)
    {
        $lineString = $this->makeLineString($pointSpecs);
        $this->assertEquals($expected, $lineString->wkt());
    }

    public function lineWkt()
    {
        return array(
            array(array(2, 3, 3, 4), "LINESTRING (2 3, 3 4)"),
            array(array(), "LINESTRING EMPTY")
        );
    }

    public function testPoints()
    {
        $p1 = new Point(3.5, 2.5);
        $p2 = new Point(1, 0);
        $p3 = new Point(5, 8);
        $lineString = new LineString($p1, $p2, $p3);
        $this->assertEquals(array($p1, $p2, $p3), $lineString->points());
        // The following assert is commented out because it causes the test suite to eventually
        // complete with this:
        //   zend_mm_heap corrupted
        //   Segmentation fault: 11
//        $this->assertEquals("org.apache.cassandra.db.marshal.LineStringType", $lineString->type()->name());
    }

    public function testPointsImmediate()
    {
        $p1 = new Point(3.5, 2.5);
        $p2 = new Point(1, 0);
        $p3 = new Point(5, 8);

        // Specify the args as new objects that no one holds references to. This has been known to
        // cause memory corruption in the past.
        $lineString = new LineString(
            new Point(3.5, 2.5),
            new Point(1, 0),
            new Point(5, 8)
        );
        $this->assertEquals(array($p1, $p2, $p3), $lineString->points());
    }

    public function testPointsIsImmutable()
    {
        $p1 = new Point(3.5, 2.5);
        $p2 = new Point(1, 0);
        $p3 = new Point(5, 8);

        $lineString = new LineString($p1, $p2, $p3);
        $points = $lineString->points();
        $this->assertEquals(3, count($points));
        $points[3] = new Point(99, 100);

        // Verify that the linestring was not affected.
        $this->assertEquals(array($p1, $p2, $p3), $lineString->points());
    }

    public function testPoint()
    {
        $lineString = new LineString(
            new Point(3.5, 2.5),
            new Point(1, 0),
            new Point(5, 8)
        );
        $this->assertEquals(new Point(1, 0), $lineString->point(1));
        $this->assertNull($lineString->point(3));
        $this->assertNull($lineString->point(-1));
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($leftPointSpecs, $rightPointSpecs, $expected)
    {
        $left = $this->makeLineString($leftPointSpecs);
        $right = $this->makeLineString($rightPointSpecs);

        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $left <=> $right);
        $this->assertEquals(-$expected, $right <=> $left);
    }

    public function comparisonTable()
    {
        // We only need to construct cases for "<" and "==" because the actual test
        // compares left to right, and right to left (and thus covers >).
        return array(
            // left has fewer points than right
            array(array(2, 3, 3, 4), array(0, 0, 0, 0, 0, 0), -1),
            // both linestrings have the same number of points, so compare each point.
            array(array(2, 3, 3, 4), array(2, 3, 3, 5), -1),
            // be sure that we compare points from left to right.
            array(array(2, 3, 3, 4), array(9, 0, 0, 0), -1),
            // equality
            array(array(2, 3, 3, 4), array(2, 3, 3, 4), 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, new LineString() <=> new Point(1, 2));
    }

    public function testProperties()
    {
        $p1 = new Point(3.5, 2.5);
        $p2 = new Point(1, 0);
        $p3 = new Point(5, 8);
        $lineString = new LineString($p1, $p2, $p3);
        $props = get_object_vars($lineString);
        $this->assertEquals(array("points" => array($p1, $p2, $p3)), $props);

        // Verify that empty linestring has an empty array of points.
        $this->assertEquals(array("points" => array()), get_object_vars(new LineString()));
    }

    /**
     * Make a LineString with the given point-specs.
     *
     * @param pointSpecs - a flat array of x,y values.
     * @return a new LineString encapsulating the specified Point's.
    */
    public function makeLineString($pointSpecs)
    {
        $iterator = new \ArrayIterator($pointSpecs);
        $points = [];
        while ($iterator->valid()) {
            $x = $iterator->current();
            $iterator->next();
            $y = $iterator->current();
            $iterator->next();

            array_push($points, new Point($x, $y));
        }
        return new LineString(...$points);
    }
}
