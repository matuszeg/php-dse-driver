<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 */

namespace Dse;

/**
 * @requires extension dse
 */
class PolygonTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be an instance of Dse\LineString, 7 given
     */
    public function testWrongTypeOfArguments()
    {
        new Polygon(new LineString(new Point(2, 3), new Point(4, 5), new Point(6,7)), 7);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Argument 2 must be a Dse\LineString with at least three points, an instance of Dse\LineString given
     */
    public function testInvalidRing()
    {
        new Polygon(new LineString(new Point(2, 3), new Point(4, 5), new Point(6,7)),
          new LineString(new Point(1, 2), new Point(2, 3)));
    }

    /**
     * @dataProvider polygonStrings
     */
    public function testToString($lineStringSpecs, $expected)
    {
        $polygon = $this->makePolygon($lineStringSpecs);
        $this->assertEquals($expected, $polygon->__toString());
    }

    public function polygonStrings()
    {
        $polyString1 = 'Exterior ring: 2,3 to 3,4 to 4,5';
        $polyString2 = 'Exterior ring: 2,3 to 3,4 to 4,5
Interior rings:
    0,0 to 1,0 to 1,1 to 1,0 to 0,0';

        return array(
            array(array(array(2, 3, 3, 4, 4, 5)), $polyString1),
            array(array(array(2, 3, 3, 4, 4, 5),
                        array(0, 0, 1, 0, 1, 1, 1, 0, 0, 0)), $polyString2),
            array(array(), "")
        );
    }

    /**
     * @dataProvider polygonWkt
     */
    public function testWkt($lineStringSpecs, $expected)
    {
        $polygon = $this->makePolygon($lineStringSpecs);
        $this->assertEquals($expected, $polygon->wkt());
    }

    public function polygonWkt()
    {
        return array(
            array(array(array(2, 3, 3, 4, 4, 5)), 'POLYGON ((2 3, 3 4, 4 5))'),
            array(array(array(2, 3, 3, 4, 4, 5),
                        array(0, 0, 1, 0, 1, 1, 1, 0, 0, 0)), 'POLYGON ((2 3, 3 4, 4 5), (0 0, 1 0, 1 1, 1 0, 0 0))'),
            array(array(), "POLYGON EMPTY")
        );
    }

    public function testExteriorRing()
    {
        $exteriorRing = new LineString(
                new Point(2, 3), new Point(3, 4), new Point(4, 5)
            );
        $polygon = $this->makeSamplePolygon();
        $this->assertEquals($exteriorRing, $polygon->exteriorRing());

        // Test that empty polygon's have a null exterior ring
        $this->assertNull((new Polygon())->exteriorRing());
    }

    public function testRings()
    {
        $exteriorRing = $this->makeLineString(2, 3, 3, 4, 4, 5);
        $interiorRing1 = $this->makeLineString(-2, -3, -3, -4, -4, -5);
        $interiorRing2 = $this->makeLineString(-4, -3, -3, -4, -4, -5);

        $polygon = new Polygon($exteriorRing, $interiorRing1, $interiorRing2);
        $this->assertEquals(array($exteriorRing, $interiorRing1, $interiorRing2), $polygon->rings());
        $this->assertEquals("org.apache.cassandra.db.marshal.PolygonType", $polygon->type()->name());
    }

    public function testInteriorRings()
    {
        $exteriorRing = $this->makeLineString(2, 3, 3, 4, 4, 5);
        $interiorRing1 = $this->makeLineString(-2, -3, -3, -4, -4, -5);
        $interiorRing2 = $this->makeLineString(-4, -3, -3, -4, -4, -5);

        $polygon = new Polygon($exteriorRing, $interiorRing1, $interiorRing2);
        $this->assertEquals(array($interiorRing1, $interiorRing2), $polygon->interiorRings());

        // Try with a polygon that has no interior rings
        $polygon = new Polygon($exteriorRing);
        $this->assertEquals(array(), $polygon->interiorRings());

        // Try with an empty polygon
        $polygon = new Polygon();
        $this->assertEquals(array(), $polygon->interiorRings());
    }

    public function testRingsImmediate()
    {
        $exteriorRing = $this->makeLineString(2, 3, 3, 4, 4, 5);
        $interiorRing1 = $this->makeLineString(-2, -3, -3, -4, -4, -5);
        $interiorRing2 = $this->makeLineString(-4, -3, -3, -4, -4, -5);

        // Specify the args as new objects that no one holds references to. This has been known to
        // cause memory corruption in the past.
        $polygon = new Polygon(
            $this->makeLineString(2, 3, 3, 4, 4, 5),
            $this->makeLineString(-2, -3, -3, -4, -4, -5),
            $this->makeLineString(-4, -3, -3, -4, -4, -5)
        );
        $this->assertEquals(array($exteriorRing, $interiorRing1, $interiorRing2), $polygon->rings());
    }

    public function testRingsIsImmutable()
    {
        $exteriorRing = $this->makeLineString(2, 3, 3, 4, 4, 5);
        $interiorRing1 = $this->makeLineString(-2, -3, -3, -4, -4, -5);
        $interiorRing2 = $this->makeLineString(-4, -3, -3, -4, -4, -5);

        $polygon = $this->makeSamplePolygon();
        $rings = $polygon->rings();
        $this->assertEquals(3, count($rings));
        $rings[3] = new LineString();

        // Verify that the polygon was not affected.
        $this->assertEquals(array($exteriorRing, $interiorRing1, $interiorRing2), $polygon->rings());
    }

    public function testRing()
    {
        $polygon = $this->makeSamplePolygon();
        $this->assertEquals($this->makeLineString(-2, -3, -3, -4, -4, -5), $polygon->ring(1));
        $this->assertNull($polygon->ring(3));
        $this->assertNull($polygon->ring(-1));
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($leftLineStringSpecs, $rightLineStringSpecs, $expected)
    {
        $left = $this->makePolygon($leftLineStringSpecs);
        $right = $this->makePolygon($rightLineStringSpecs);

        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $left <=> $right);
        $this->assertEquals(-$expected, $right <=> $left);
    }

    public function comparisonTable()
    {
        // We only need to construct cases for "<" and "==" because the actual test
        // compares left to right and right to left (and thus covers >).
        return array(
            // left has fewer line-strings than right
            array(array(array(9, 9, 9, 9, 9, 9)), array(array(1, 2, 3, 4, 5, 6), array(1, 2, 3, 4, 5, 6)), -1),
            // both polygons have the same number of line-strings, so compare each line-string.
            array(array(array(2, 3, 3, 4, 4, 5), array(9, 9, 9, 9, 9, 9)), array(array(2, 3, 3, 4, 4, 5), array(9, 9, 9, 9, 9, 10)), -1),
            // be sure that we compare line-strings from left to right.
            array(array(array(2, 3, 3, 4, 4, 5), array(9, 9, 9, 9, 9, 9)), array(array(5, 3, 3, 4, 4, 5), array(0, 0, 0, 0, 0, 0)), -1),
            // equality
            array(array(array(2, 3, 3, 4, 4, 5), array(9, 9, 9, 9, 9, 9)), array(array(2, 3, 3, 4, 4, 5), array(9, 9, 9, 9, 9, 9)), 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, new Polygon() <=> new Point(1, 2));
    }

    public function testProperties()
    {
        $polygon = $this->makeSamplePolygon();
        $props = get_object_vars($polygon);
        $this->assertEquals(array("rings" => array(
            $this->makeLineString(2, 3, 3, 4, 4, 5),
            $this->makeLineString(-2, -3, -3, -4, -4, -5),
            $this->makeLineString(-4, -3, -3, -4, -4, -5)
        )), $props);

        // Verify that empty linestring has an empty array of points.
        $this->assertEquals(array("rings" => array()), get_object_vars(new Polygon()));
    }

    /**
     * Make a LineString with the given point-specs.
     *
     * @param pointSpecs - a flat array of x,y values.
     * @return a new LineString encapsulating the specified Point's.
    */
    public function makeLineString(...$pointSpecs)
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

    /**
     * Make a Polygon with the given line-string-specs.
     *
     * @param lineStringSpecs - an array of arrays. The outer array elements represent
     *             line-strings. The inner array elements represent points within a line-string.
     * @return a new Polygon encapsulating the specified LineString's.
    */
    public function makePolygon($lineStringSpecs)
    {
        $iterator = new \ArrayIterator($lineStringSpecs);

        $lineStrings = [];
        foreach ($lineStringSpecs as $lineString) {
           array_push($lineStrings, $this->makeLineString(...$lineString));
        }
        return new Polygon(...$lineStrings);
    }

    public function makeSamplePolygon()
    {
        return $this->makePolygon(
            array(
                array(2, 3, 3, 4, 4, 5),
                array(-2, -3, -3, -4, -4, -5),
                array(-4, -3, -3, -4, -4, -5)
            )
        );
    }
}
