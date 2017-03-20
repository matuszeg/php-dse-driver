# 1.0.0

Features:

* Added support to pass a query string  directly to `Session::execute()` and
  `Session::executeAsync()`
* `ExecutionOptions` has been deprecated in favor of using an array

Bug Fixes:

* [PHP-86] Decimal __toString method
* [PHP-109] Cassandra solr_query not working after upgrading to DSE 5
* [PHP-114] pecl install compiler output is dominated by C90 warnings
* [PHP-119] Driver crashes upon getting type of column
* [PHP-143] Memory leak in `FutureSession`
* [PHP-144] Fix persistent sessions reference counting issue
* [PHP-147] Unable to create Bigint with PHP_INT_MIN

# 1.0.0-rc1

Features:

* Added support for DSE Graph. Queries can be run using new methods added to
  `Dse\Session`: `Dse\Session::executeGraph()` and `Dse\Session::executeGraphAsync()`
* Added support for DSE geospatial types: `Dse\Point`, `Dse\LineString` and
  `Dse\Polygon`.
* Added support for DSE Unified Authentication. It is configure by building a
 `Dse\Cluster` object with the `Dse\Cluster\Builder::withGssapiAuthenticator()` and/or
 `Dse\Cluster\Builder::withPlaintextAuthenticator()` methods.
