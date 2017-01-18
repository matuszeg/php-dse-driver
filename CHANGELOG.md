# 1.0.0-rc1

Features:

* Added support for DSE Graph. Queries can be run using new methods added to
  `Dse\Session`: `Dse\Session::executeGraph()` and `Dse\Session::executeGraphAsync()`
* Added support for DSE geospatial types: `Dse\Point`, `Dse\LineString` and
  `Dse\Polygon`.
* Added support for DSE Unified Authentication. It is configure by building a
 `Dse\Cluster` object with the `Dse\Cluster\Builder::withGssapiAuthenticator()` and/or
 `Dse\Cluster\Builder::withPlaintextAuthenticator()` methods.
