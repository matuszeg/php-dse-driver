# Proxy Authentication and Execution

## Proxy Execution
Proxy execution allows a client to connect to a node as one user but run queries as a different user.

For example, if a webapp accesses DSE as the 'service' user, but needs to issue some queries as the end user 'bob',
a DSE admin would first set up permissions in DSE to allow the 'service' user to execute queries as 'bob':

```
GRANT PROXY.EXECUTE ON ROLE bob TO service;
```

To run a statement as 'bob', the client simply sets the `execute_as` execution option when executing the statement:

```php
$result = $session->execute($statement, array("execute_as" => "bob"));
```

## Proxy Authentication
Proxy authentication allows a client to connect to a node as one user but declare that all actions of the client should
actually run as a different user (without needing credentials of that second user).

For example, if a webapp accesses DSE as the 'service' user, but needs to issue all queries as the end user 'bob',
a DSE admin would first set up permissions in DSE to allow the 'service' user to proxy login as 'bob':

```
GRANT PROXY.LOGIN ON ROLE bob TO service;
```

Then the client authenticates with DSE as follows:

```php
// A DSE cluster using plain-text authentication would use:
$cluster = Dse::cluster()
          ->withPlaintextAuthenticator("service_user", "service_user", "bob")
          ->build();

// A DSE cluster using GSSAPI authentication would use:
$cluster = Dse::cluster()
          ->withGssapiAuthenticator("dse", "service@DATASTAX.COM", "bob")
          ->build();
```

Note that if DSE is set up to leverage multiple authentication systems, the authenticated user may come from one system
(e.g. Kerberos) and the proxied user from another (e.g. internal).