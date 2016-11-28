PHP_ARG_WITH(dse, Enable Cassandra extension,
[  --with-dse[=DIR]    Enable the DataStax Enterprise extension.])

AC_MSG_CHECKING([for supported PHP version])
PHP_DSE_FOUND_PHP_VERSION=`${PHP_CONFIG} --version`
PHP_DSE_FOUND_PHP_VERSION_NUMBER=`echo "${PHP_DSE_FOUND_PHP_VERSION}" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
if test "$PHP_DSE_FOUND_PHP_VERSION_NUMBER" -lt "50500"; then
  AC_MSG_ERROR([not supported. PHP version 5.5.0+ required (found $PHP_DSE_FOUND_PHP_VERSION)])
else
  AC_MSG_RESULT([supported ($PHP_DSE_FOUND_PHP_VERSION)])
fi

if test -z "$PHP_UV"; then
  PHP_ARG_WITH(uv, libuv install dir,
  [  --with-uv=DIR           Specify libuv install dir.])
fi

if test -z "$PHP_GMP"; then
  PHP_ARG_WITH(gmp, GNU MP install dir,
  [  --with-gmp=DIR          Specify GNU MP install dir.])
fi

if test "$PHP_DSE" != "no"; then
  PHP_DRIVER_DIR="php-driver/ext"

  CASSANDRA_CLASSES="\
    $PHP_DRIVER_DIR/src/Cassandra.c \
    $PHP_DRIVER_DIR/src/Cassandra/Aggregate.c \
    $PHP_DRIVER_DIR/src/Cassandra/BatchStatement.c \
    $PHP_DRIVER_DIR/src/Cassandra/Bigint.c \
    $PHP_DRIVER_DIR/src/Cassandra/Blob.c \
    $PHP_DRIVER_DIR/src/Cassandra/Cluster.c \
    $PHP_DRIVER_DIR/src/Cassandra/Cluster/Builder.c \
    $PHP_DRIVER_DIR/src/Cassandra/Cluster/DefaultBuilder.c \
    $PHP_DRIVER_DIR/src/Cassandra/Collection.c \
    $PHP_DRIVER_DIR/src/Cassandra/Column.c \
    $PHP_DRIVER_DIR/src/Cassandra/Custom.c \
    $PHP_DRIVER_DIR/src/Cassandra/Date.c \
    $PHP_DRIVER_DIR/src/Cassandra/Decimal.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultAggregate.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultCluster.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultColumn.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultFunction.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultIndex.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultKeyspace.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultMaterializedView.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultSchema.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultSession.c \
    $PHP_DRIVER_DIR/src/Cassandra/DefaultTable.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception.c \
    $PHP_DRIVER_DIR/src/Cassandra/ExecutionOptions.c \
    $PHP_DRIVER_DIR/src/Cassandra/Float.c \
    $PHP_DRIVER_DIR/src/Cassandra/Function.c \
    $PHP_DRIVER_DIR/src/Cassandra/Future.c \
    $PHP_DRIVER_DIR/src/Cassandra/FutureClose.c \
    $PHP_DRIVER_DIR/src/Cassandra/FuturePreparedStatement.c \
    $PHP_DRIVER_DIR/src/Cassandra/FutureRows.c \
    $PHP_DRIVER_DIR/src/Cassandra/FutureSession.c \
    $PHP_DRIVER_DIR/src/Cassandra/FutureValue.c \
    $PHP_DRIVER_DIR/src/Cassandra/Index.c \
    $PHP_DRIVER_DIR/src/Cassandra/Inet.c \
    $PHP_DRIVER_DIR/src/Cassandra/Keyspace.c \
    $PHP_DRIVER_DIR/src/Cassandra/Map.c \
    $PHP_DRIVER_DIR/src/Cassandra/MaterializedView.c \
    $PHP_DRIVER_DIR/src/Cassandra/Numeric.c \
    $PHP_DRIVER_DIR/src/Cassandra/PreparedStatement.c \
    $PHP_DRIVER_DIR/src/Cassandra/RetryPolicy.c \
    $PHP_DRIVER_DIR/src/Cassandra/Rows.c \
    $PHP_DRIVER_DIR/src/Cassandra/Schema.c \
    $PHP_DRIVER_DIR/src/Cassandra/Session.c \
    $PHP_DRIVER_DIR/src/Cassandra/Set.c \
    $PHP_DRIVER_DIR/src/Cassandra/SimpleStatement.c \
    $PHP_DRIVER_DIR/src/Cassandra/Smallint.c \
    $PHP_DRIVER_DIR/src/Cassandra/SSLOptions.c \
    $PHP_DRIVER_DIR/src/Cassandra/SSLOptions/Builder.c \
    $PHP_DRIVER_DIR/src/Cassandra/Statement.c \
    $PHP_DRIVER_DIR/src/Cassandra/Table.c \
    $PHP_DRIVER_DIR/src/Cassandra/Time.c \
    $PHP_DRIVER_DIR/src/Cassandra/Timestamp.c \
    $PHP_DRIVER_DIR/src/Cassandra/TimestampGenerator.c \
    $PHP_DRIVER_DIR/src/Cassandra/TimestampGenerator/Monotonic.c \
    $PHP_DRIVER_DIR/src/Cassandra/TimestampGenerator/ServerSide.c \
    $PHP_DRIVER_DIR/src/Cassandra/Timeuuid.c \
    $PHP_DRIVER_DIR/src/Cassandra/Tinyint.c \
    $PHP_DRIVER_DIR/src/Cassandra/Tuple.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type.c \
    $PHP_DRIVER_DIR/src/Cassandra/UserTypeValue.c \
    $PHP_DRIVER_DIR/src/Cassandra/Uuid.c \
    $PHP_DRIVER_DIR/src/Cassandra/UuidInterface.c \
    $PHP_DRIVER_DIR/src/Cassandra/Value.c \
    $PHP_DRIVER_DIR/src/Cassandra/Varint.c \
  ";

  CASSANDRA_EXCEPTIONS="\
    $PHP_DRIVER_DIR/src/Cassandra/Exception/AlreadyExistsException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/AuthenticationException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ConfigurationException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/DivideByZeroException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/DomainException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ExecutionException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/InvalidArgumentException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/InvalidQueryException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/InvalidSyntaxException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/IsBootstrappingException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/LogicException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/OverloadedException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ProtocolException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/RangeException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ReadTimeout.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/RuntimeException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ServerException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/TimeoutException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/TruncateException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/UnauthorizedException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/UnavailableException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/UnpreparedException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/ValidationException.c \
    $PHP_DRIVER_DIR/src/Cassandra/Exception/WriteTimeoutException.c
  ";

  CASSANDRA_RETRY_POLICIES="\
    $PHP_DRIVER_DIR/src/Cassandra/RetryPolicy/DefaultPolicy.c \
    $PHP_DRIVER_DIR/src/Cassandra/RetryPolicy/DowngradingConsistency.c \
    $PHP_DRIVER_DIR/src/Cassandra/RetryPolicy/Fallthrough.c \
    $PHP_DRIVER_DIR/src/Cassandra/RetryPolicy/Logging.c
  ";

  CASSANDRA_TYPES="\
    $PHP_DRIVER_DIR/src/Cassandra/Type/Collection.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/Custom.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/Map.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/Scalar.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/Set.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/Tuple.c \
    $PHP_DRIVER_DIR/src/Cassandra/Type/UserType.c
  ";

  CASSANDRA_UTIL="\
    $PHP_DRIVER_DIR/util/bytes.c \
    $PHP_DRIVER_DIR/util/collections.c \
    $PHP_DRIVER_DIR/util/consistency.c \
    $PHP_DRIVER_DIR/util/future.c \
    $PHP_DRIVER_DIR/util/hash.c \
    $PHP_DRIVER_DIR/util/inet.c \
    $PHP_DRIVER_DIR/util/math.c \
    $PHP_DRIVER_DIR/util/ref.c \
    $PHP_DRIVER_DIR/util/result.c \
    $PHP_DRIVER_DIR/util/types.c \
    $PHP_DRIVER_DIR/util/uuid_gen.c \
  ";

  DSE_CLASSES="\
    src/Dse.c \
    src/Dse/Cluster.c \
    src/Dse/Cluster/Builder.c \
    src/Dse/Cluster/DefaultBuilder.c \
    src/Dse/DefaultCluster.c \
    src/Dse/DefaultSession.c \
    src/Dse/FutureSession.c \
    src/Dse/Graph/DefaultEdge.c \
    src/Dse/Graph/DefaultElement.c \
    src/Dse/Graph/DefaultPath.c \
    src/Dse/Graph/DefaultProperty.c \
    src/Dse/Graph/DefaultVertex.c \
    src/Dse/Graph/DefaultVertexProperty.c \
    src/Dse/Graph/Edge.c \
    src/Dse/Graph/Element.c \
    src/Dse/Graph/FutureResultSet.c \
    src/Dse/Graph/Options.c \
    src/Dse/Graph/Path.c \
    src/Dse/Graph/Property.c \
    src/Dse/Graph/Result.c \
    src/Dse/Graph/ResultSet.c \
    src/Dse/Graph/SimpleStatement.c \
    src/Dse/Graph/Statement.c \
    src/Dse/Graph/Vertex.c \
    src/Dse/Graph/VertexProperty.c \
    src/Dse/LineString.c \
    src/Dse/Point.c \
    src/Dse/Polygon.c \
    src/Dse/Session.c
  ";

  case $(uname -s) in
    Linux)
      DSE_CFLAGS="-Wall -pedantic -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi -pthread"
      ;;
    Darwin)
      DSE_CFLAGS="-Wall -pedantic -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi"
      ;;
  esac

  PHP_NEW_EXTENSION(dse,
    php_dse.c \
    $DSE_CLASSES \
    $PHP_DRIVER_DIR/php_cassandra_shared.c \
    $CASSANDRA_CLASSES \
    $CASSANDRA_EXCEPTIONS \
    $CASSANDRA_RETRY_POLICIES \
    $CASSANDRA_TYPES \
    $CASSANDRA_UTIL,
    $ext_shared, , $DSE_CFLAGS)
  PHP_ADD_BUILD_DIR($ext_builddir/src)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/Cluster)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/Exception)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/SSLOptions)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/Type)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/RetryPolicy)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/src/Cassandra/TimestampGenerator)
  PHP_ADD_BUILD_DIR($PHP_DRIVER_DIR/util)
  PHP_SUBST(DSE_SHARED_LIBADD)
  PHP_SUBST(DSE_CFLAGS)

  PHP_ADD_INCLUDE($ext_srcdir/$PHP_DRIVER_DIR)
  PHP_ADD_INCLUDE($ext_srcdir/$PHP_DRIVER_DIR/src)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(dse, spl)
  ])

  if test "$PHP_GMP" != "no"; then
    if test -f $PHP_GMP/include/gmp.h; then
      GMP_DIR=$PHP_GMP
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/gmp.h; then
        GMP_DIR=$i
      fi
    done
  fi

  if test -z "$GMP_DIR"; then
    ac_extra=
  else
    ac_extra=-L$GMP_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(gmp, __gmp_version,
    [
      AC_DEFINE(HAVE_GMPLIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libgmp])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$GMP_DIR"; then
    PHP_ADD_LIBPATH($GMP_DIR/$PHP_LIBDIR, DSE_SHARED_LIBADD)
    PHP_ADD_INCLUDE($GMP_DIR/include)
  fi

  PHP_ADD_LIBRARY(gmp,, DSE_SHARED_LIBADD)

  if test "$PHP_UV" != "no"; then
    if test -f $PHP_UV/include/uv.h; then
      UV_DIR=$PHP_UV
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/uv.h; then
        UV_DIR=$i
      fi
    done
  fi

  if test -z "$UV_DIR"; then
    ac_extra=
  else
    ac_extra=-L$UV_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(uv, uv_version,
    [
      AC_DEFINE(HAVE_UVLIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libuv])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$UV_DIR"; then
    PHP_ADD_LIBPATH($UV_DIR/$PHP_LIBDIR, DSE_SHARED_LIBADD)
    PHP_ADD_INCLUDE($UV_DIR/include)
  fi

  PHP_ADD_LIBRARY(uv,, DSE_SHARED_LIBADD)
  PHP_ADD_LIBRARY(m,, DSE_SHARED_LIBADD)

  if test "$PHP_DSE" != "yes"; then
    if test -f $PHP_DSE/include/dse.h; then
      CPP_DRIVER_DIR=$PHP_DSE
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/dse.h; then
        CPP_DRIVER_DIR=$i
      fi
    done
  fi

  AC_MSG_CHECKING([for supported DataStax Enterprise C/C++ driver version])
  PHP_DSE_FOUND_CASSANDRA_VERSION=`$AWK '/DSE_VERSION_MAJOR/ {printf $3"."} /DSE_VERSION_MINOR/ {printf $3"."} /DSE_VERSION_PATCH/ {printf $3}' $CPP_DRIVER_DIR/include/dse.h`
  PHP_DSE_FOUND_CASSANDRA_VERSION_NUMBER=`echo "${PHP_DSE_FOUND_CASSANDRA_VERSION}" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
  if test "$PHP_DSE_FOUND_CASSANDRA_VERSION_NUMBER" -lt "10000"; then
  AC_MSG_ERROR([not supported. Driver version 1.0.0+ required (found $PHP_DSE_FOUND_CASSANDRA_VERSION)])
  else
    AC_MSG_RESULT([supported ($PHP_DSE_FOUND_CASSANDRA_VERSION)])
  fi

  if test -z "$CPP_DRIVER_DIR"; then
    ac_extra=
  else
    ac_extra=-L$CPP_DRIVER_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(dse, cass_cluster_new_dse,
    [
      AC_DEFINE(HAVE_DSELIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libdse])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$CPP_DRIVER_DIR"; then
    PHP_ADD_LIBPATH($CPP_DRIVER_DIR/$PHP_LIBDIR, DSE_SHARED_LIBADD)
    PHP_ADD_INCLUDE($CPP_DRIVER_DIR/include)
  fi

  PHP_ADD_LIBRARY(dse,, DSE_SHARED_LIBADD)

  DSE_SHARED_LIBADD="$DSE_SHARED_LIBADD $LIBS"
fi
