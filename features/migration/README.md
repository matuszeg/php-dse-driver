# Migration

When using DataStax Enterprise (DSE) it is highly recommend that your
application use the DataStax Enterprise Driver instead of using the DataStax
Driver for Apache Cassandra. This will allow your application to take full
advantage of DSE. Migrating your application is easy and namespace aliasing can
allow your existing code to remain mostly unchanged.

```php
use Dse as Cassandra;

# Existing code
```

This should work for most code bases, but this has a couple limitations:

* Code cannot use fully qualified names when using aliasing (e.g.
  `\Cassandra\SimpleStatement`)
* Code that uses `is_a()` for objects in the `Cassandra` namespace  will need to
  use `instanceof` instead.

  ```php
  use Dse as Cassandra;

  $value = new Cassandra\Float(1);

  echo "is_a(\$value, 'Cassandra\Float')? " . (is_a($value, "Cassandra\Float") ?
                                               "true" : "false") . PHP_EOL;
  echo "is_a(\$value, Dse\Float)'? " . (is_a($value, "Dse\Float") ?
                                        "true" : "false") . PHP_EOL;
  echo "\$value instaneof 'Cassandra\Float'? " . ($value instanceof Cassandra\Float ?
                                                  "true" : "false") . PHP_EOL;
  echo "\$value instaneof 'Dse\Float'? " . ($value instanceof Dse\Float ?
                                            "true" : "false") . PHP_EOL;
  ```
