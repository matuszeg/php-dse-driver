Feature: Logging

  PHP Driver support file logging and system logging

  Background:
    Given a running Cassandra cluster

  Scenario: Changing PHP driver logging configuration
     Given the following logger settings:
      """ini
      dse.log=feature-logging.log
      dse.log_level=TRACE
      """
    And the following example:
      """php
      $cluster   = Dse::cluster()->build();
      $session   = $cluster->connect();
      """
    When it is executed
    Then a log file "feature-logging.log" should exist
    And the log file "feature-logging.log" should contain "TRACE"
