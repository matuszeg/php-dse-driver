# DateRange

DataStax Enterprise 5.1+ introduces a `DateRange` type to represent ranges of dates
and times. For example:

* from 1999 to March 2003
* from 4/17/2002 18:36 to 4/19/2002 18:37:30.5
* from 9/25/2013 and never ending
* from the beginning of time until 9/25/2013
* on 12/25/2005

One end of a DateRange (called a *bound*) consists of a millisecond-precision
timestamp and a desired precision. Thus, a bound of **1999** is represented by
a timestamp for any time in the year 1999 and a precision of **year**.

Three classes in the driver model this structure:

* [`Dse\DateRange\Precision`](/api/Dse/DateRange/class.Precision/)
* [`Dse\DateRange\Bound`](/api/Dse/DateRange/class.Bound/),
* [`Dse\DateRange`](/api/Dse/class.DateRange/),

## Precision
The [`Precision`](/api/Dse/DateRange/class.Precision/) class contains constants
for the valid precisions:

* YEAR
* MONTH
* DAY
* HOUR
* MINUTE
* SECOND
* MILLISECOND

This class cannot be instantiated.

## Bound
The [`Bound`](/api/Dse/DateRange/class.Bound/) class encapsulates one end 
of a date-range. It consists of an integer timestamp (obtained via the 
([`timeMs()`](/api/Dse/DateRange/class.Bound/#method-timeMs)) accessor) and a
[`Precision`](/api/Dse/DateRange/class.Precision/) (obtained via the
[`precision()`](/api/Dse/DateRange/class.Bound/#method-precision) accessor). The
timestamp is the number of milliseconds since the start of the epoch.

This class also has a static method called `unbounded()` which returns a `Bound`
that represents "no bound". It is used as the virtual end-point of an open-ended
range.

To check if a bound is unbounded, simply compare to `Bound::unbounded()`:

```php
if ($b == Dse\DateRange\Bound::unbounded()) {
...
}
```

The constructor for `Bound` accepts `long`, `double`, [`Dse\Bigint`](/api/Dse/class.Bigint), numeric string, 
and `DateTime` values for the timestamp attribute:

```php
use Dse\DateRange\Precision;
use Dse\DateRange\Bound;

$b = new Bound(Precision::YEAR, 12345);
$b = new Bound(Precision::YEAR, "12345");
$b = new Bound(Precision::YEAR, new DateTime("2016-12-17"));
```

This is particularly useful on 32-bit platforms where PHP does not inherently support 64-bit integers.

## DateRange
A DateRange typically has a lower-bound (`lowerBound()`) and an upper-bound (`upperBound()`) such as
"from 4/17/2002 18:36 to 4/19/2002 18:37:30.5". Open-ended ranges have a `Bound::unbounded()` on
the open-side.

However, date ranges can also consist of a single value, as in "on 12/25/2005". Such DateRange
objects store the single bound in `lowerBound()`. `upperBound()` returns `NULL`. In addition, an
`isSingleDate()` function returns whether or not this DateRange has a single date value.

The constructor for `DateRange` accepts both `Bound` and `<Precision, timestamp>` pairs, where
`timestamp` may be a `long`, `double`, [`Dse\Bigint`](/api/Dse/class.Bigint), numeric string, 
or `DateTime`.
 
Putting all of this together, one can manipulate DateRange objects as follows:

```php
use Dse\DateRange;
use Dse\DateRange\Precision;
use Dse\DateRange\Bound;

// long timestamp (seconds precision) representing 2 days after the beginning of the epoch 
$t = 86400 * 2;

// Single date
$d = new DateRange(Precision::YEAR, $time * 1000);
$d = new DateRange(Precision::YEAR, new DateTime("2005-04-03"));

// Single date, using a DateRange\Bound object.
$bound = new Bound(Precision::YEAR, new DateTime("2005-04-03"));
$d = new DateRange($bound);

// Closed range
$d = new DateRange(Precision::YEAR, ($t - 3600) * 1000, Precision::DAY, $t * 1000); 

// Open value
$d = new DateRange(Bound::unbounded());

// Open range on one side.
$d = new DateRange(Precision::MILLISECOND, $t, Bound::unbounded());

// Pull out attributes
print $d->lowerBound()->precision();
print $d->upperBound()->timeMs();

// Check if unbounded on one side
if ($d->lowerBound() == Bound::unbounded()) {
   …
}
```