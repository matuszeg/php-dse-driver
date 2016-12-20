PHP_ARG_WITH(dse, Enable DSE extension,
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
  PHP_DRIVER_CLASSES="\
    src/Aggregate.c \
    src/BatchStatement.c \
    src/Bigint.c \
    src/Blob.c \
    src/Cluster.c \
    src/Cluster/Builder.c \
    src/Collection.c \
    src/Column.c \
    src/Core.c \
    src/Custom.c \
    src/Date.c \
    src/Decimal.c \
    src/DefaultAggregate.c \
    src/DefaultCluster.c \
    src/DefaultColumn.c \
    src/DefaultFunction.c \
    src/DefaultIndex.c \
    src/DefaultKeyspace.c \
    src/DefaultMaterializedView.c \
    src/DefaultSchema.c \
    src/DefaultSession.c \
    src/DefaultTable.c \
    src/Exception.c \
    src/ExecutionOptions.c \
    src/Float.c \
    src/Function.c \
    src/Future.c \
    src/FutureClose.c \
    src/FuturePreparedStatement.c \
    src/FutureRows.c \
    src/FutureSession.c \
    src/FutureValue.c \
    src/Graph/DefaultEdge.c \
    src/Graph/DefaultElement.c \
    src/Graph/DefaultPath.c \
    src/Graph/DefaultProperty.c \
    src/Graph/DefaultVertex.c \
    src/Graph/DefaultVertexProperty.c \
    src/Graph/Edge.c \
    src/Graph/Element.c \
    src/Graph/FutureResultSet.c \
    src/Graph/Options.c \
    src/Graph/Options/Builder.c \
    src/Graph/Path.c \
    src/Graph/Property.c \
    src/Graph/Result.c \
    src/Graph/ResultSet.c \
    src/Graph/SimpleStatement.c \
    src/Graph/Statement.c \
    src/Graph/Vertex.c \
    src/Graph/VertexProperty.c \
    src/Index.c \
    src/Inet.c \
    src/Keyspace.c \
    src/LineString.c \
    src/Map.c \
    src/MaterializedView.c \
    src/Numeric.c \
    src/Point.c \
    src/Polygon.c \
    src/PreparedStatement.c \
    src/RetryPolicy.c \
    src/Rows.c \
    src/Schema.c \
    src/Session.c \
    src/Set.c \
    src/SimpleStatement.c \
    src/Smallint.c \
    src/SSLOptions.c \
    src/SSLOptions/Builder.c \
    src/Statement.c \
    src/Table.c \
    src/Time.c \
    src/Timestamp.c \
    src/TimestampGenerator.c \
    src/TimestampGenerator/Monotonic.c \
    src/TimestampGenerator/ServerSide.c \
    src/Timeuuid.c \
    src/Tinyint.c \
    src/Tuple.c \
    src/Type.c \
    src/UserTypeValue.c \
    src/Uuid.c \
    src/UuidInterface.c \
    src/Value.c \
    src/Varint.c
  ";

  PHP_DRIVER_EXCEPTIONS="\
    src/Exception/AlreadyExistsException.c \
    src/Exception/AuthenticationException.c \
    src/Exception/ConfigurationException.c \
    src/Exception/DivideByZeroException.c \
    src/Exception/DomainException.c \
    src/Exception/ExecutionException.c \
    src/Exception/InvalidArgumentException.c \
    src/Exception/InvalidQueryException.c \
    src/Exception/InvalidSyntaxException.c \
    src/Exception/IsBootstrappingException.c \
    src/Exception/LogicException.c \
    src/Exception/OverloadedException.c \
    src/Exception/ProtocolException.c \
    src/Exception/RangeException.c \
    src/Exception/ReadTimeoutException.c \
    src/Exception/RuntimeException.c \
    src/Exception/ServerException.c \
    src/Exception/TimeoutException.c \
    src/Exception/TruncateException.c \
    src/Exception/UnauthorizedException.c \
    src/Exception/UnavailableException.c \
    src/Exception/UnpreparedException.c \
    src/Exception/ValidationException.c \
    src/Exception/WriteTimeoutException.c
  ";

  PHP_DRIVER_RETRY_POLICIES="\
    src/RetryPolicy/DefaultPolicy.c \
    src/RetryPolicy/DowngradingConsistency.c \
    src/RetryPolicy/Fallthrough.c \
    src/RetryPolicy/Logging.c
  ";

  PHP_DRIVER_TYPES="\
    src/Type/Collection.c \
    src/Type/Custom.c \
    src/Type/Map.c \
    src/Type/Scalar.c \
    src/Type/Set.c \
    src/Type/Tuple.c \
    src/Type/UserType.c
  ";

  PHP_DRIVER_UTIL="\
    util/bytes.c \
    util/collections.c \
    util/consistency.c \
    util/future.c \
    util/hash.c \
    util/inet.c \
    util/math.c \
    util/ref.c \
    util/result.c \
    util/types.c \
    util/uuid_gen.c \
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
    php_driver.c \
    $PHP_DRIVER_CLASSES \
    $PHP_DRIVER_EXCEPTIONS \
    $PHP_DRIVER_RETRY_POLICIES \
    $PHP_DRIVER_TYPES \
    $PHP_DRIVER_UTIL,
    $ext_shared, , $DSE_CFLAGS)
  PHP_ADD_BUILD_DIR($ext_builddir/src)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Cluster)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Graph)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Graph/Options)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Exception)
  PHP_ADD_BUILD_DIR($ext_builddir/src/SSLOptions)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Type)
  PHP_ADD_BUILD_DIR($ext_builddir/src/RetryPolicy)
  PHP_ADD_BUILD_DIR($ext_builddir/src/TimestampGenerator)
  PHP_ADD_BUILD_DIR($ext_builddir/util)
  PHP_SUBST(DSE_SHARED_LIBADD)
  PHP_SUBST(DSE_CFLAGS)

  PHP_ADD_INCLUDE($ext_srcdir/$PHP_DRIVER_DIR)
 # PHP_ADD_INCLUDE($ext_srcdir/$PHP_DRIVER_DIR/src)

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
  PHP_DSE_FOUND_VERSION=`$AWK '/DSE_VERSION_MAJOR/ {printf $3"."} /DSE_VERSION_MINOR/ {printf $3"."} /DSE_VERSION_PATCH/ {printf $3}' $CPP_DRIVER_DIR/include/dse.h`
  PHP_DSE_FOUND_VERSION_NUMBER=`echo "${PHP_DSE_FOUND_VERSION}" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
  if test "$PHP_DSE_FOUND_VERSION_NUMBER" -lt "10100"; then
  AC_MSG_ERROR([not supported. Driver version 1.1.0+ required (found $PHP_DSE_FOUND_VERSION)])
  else
    AC_MSG_RESULT([supported ($PHP_DSE_FOUND_VERSION)])
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
