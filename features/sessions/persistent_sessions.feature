@web_server
Feature: Persistent Sessions

  PHP Driver sessions can persist

  Background:
    Given a running Cassandra cluster
    And a URI "status.php" with:
      """php
      echo phpinfo();
      """

  Scenario: No sessions have been created
    When I go to "/status.php"
    Then I should see:
      | Persistent Clusters | 0 |
      | Persistent Sessions | 0 |

  Scenario: A single session is re-used for all requests
    Given a URI "connect.php" with:
      """php
      $cluster = Dse::cluster()
                     ->withPersistentSessions(true)
                     ->build();
      $session = $cluster->connect();
      """
    When I go to "/connect.php" 5 times
    When I go to "/status.php"
    Then I should see:
      | Persistent Clusters | 1 |
      | Persistent Sessions | 1 |

  Scenario: Multiple persistent sessions are used for requests
    Given a URI "connect.php" with:
      """php
      $cluster = Dse::cluster()
                     ->withPersistentSessions(true)
                     ->build();
      $session = $cluster->connect();
      """
    And a URI "connect_system.php" with:
      """php
      $cluster = Dse::cluster()
                     ->withPersistentSessions(true)
                     ->build();
      $session = $cluster->connect("system");
      """
    When I go to "/connect.php"
    When I go to "/connect_system.php"
    When I go to "/status.php"
    Then I should see:
      | Persistent Clusters | 1 |
      | Persistent Sessions | 2 |

  Scenario: Non-persistent sessions are recreated for each request
    Given a URI "connect.php" with:
      """php
      $cluster = Dse::cluster()
                     ->withPersistentSessions(false)
                     ->build();
      $session = $cluster->connect();
      """
    When I go to "/connect.php"
    When I go to "/status.php"
    Then I should see:
      | Persistent Clusters | 0 |
      | Persistent Sessions | 0 |
