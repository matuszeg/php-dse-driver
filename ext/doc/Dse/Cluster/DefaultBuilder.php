<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Cluster;

/**
 */
final class DefaultBuilder extends Builder  {

    /**
     * @return mixed
     */
    public function build() { }

    /**
     * @return mixed
     */
    public function withDefaultConsistency($consistency) { }

    /**
     * @return mixed
     */
    public function withDefaultPageSize($pageSize) { }

    /**
     * @return mixed
     */
    public function withDefaultTimeout($timeout) { }

    /**
     * @return mixed
     */
    public function withContactPoints($host) { }

    /**
     * @return mixed
     */
    public function withPort($port) { }

    /**
     * @return mixed
     */
    public function withRoundRobinLoadBalancingPolicy() { }

    /**
     * @return mixed
     */
    public function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies) { }

    /**
     * @return mixed
     */
    public function withBlackListHosts($hosts) { }

    /**
     * @return mixed
     */
    public function withWhiteListHosts($hosts) { }

    /**
     * @return mixed
     */
    public function withBlackListDCs($dcs) { }

    /**
     * @return mixed
     */
    public function withWhiteListDCs($dcs) { }

    /**
     * @return mixed
     */
    public function withTokenAwareRouting($enabled) { }

    /**
     * @return mixed
     */
    public function withCredentials($username, $password) { }

    /**
     * @return mixed
     */
    public function withConnectTimeout($timeout) { }

    /**
     * @return mixed
     */
    public function withRequestTimeout($timeout) { }

    /**
     * @return mixed
     */
    public function withSSL($options) { }

    /**
     * @return mixed
     */
    public function withPersistentSessions($enabled) { }

    /**
     * @return mixed
     */
    public function withProtocolVersion($version) { }

    /**
     * @return mixed
     */
    public function withIOThreads($count) { }

    /**
     * @return mixed
     */
    public function withConnectionsPerHost($core, $max) { }

    /**
     * @return mixed
     */
    public function withReconnectInterval($interval) { }

    /**
     * @return mixed
     */
    public function withLatencyAwareRouting($enabled) { }

    /**
     * @return mixed
     */
    public function withTCPNodelay($enabled) { }

    /**
     * @return mixed
     */
    public function withTCPKeepalive($delay) { }

    /**
     * @return mixed
     */
    public function withRetryPolicy($policy) { }

    /**
     * @return mixed
     */
    public function withTimestampGenerator($generator) { }

    /**
     * @return mixed
     */
    public function withSchemaMetadata($enabled) { }

    /**
     * Enables/Disables Hostname Resolution.
     *
     * If enabled the driver will resolve hostnames for IP addresses using
     * reverse IP lookup. This is useful for authentication (Kerberos) or
     * encryption SSL services that require a valid hostname for verification.
     *
     * param bool $enabled True to use hostname resolution; false otherwise.
     * @return DefaultBuilder self
     */
    public function withHostnameResolution($enabled) { }

    /**
     * Enable/Disables Randomized Contact Points
     *
     * If enabled this allows the driver randomly use contact points in order
     * to evenly spread the load across the cluster and prevent
     * hotspots/load spikes during notifications (e.g. massive schema change).
     *
     * Note: This setting should only be disabled for debugging and testing.
     *
     * param bool $enabled True to enable randomized contact points; false otherwise.
     * @return DefaultBuilder self
     */
    public function withRandomizedContactPoints($enabled) { }

    /**
     * Specify interval in seconds that the driver should wait before attempting
     * to send heartbeat messages and control the amount of time the connection
     * must be idle before sending heartbeat messages. This is useful for
     * preventing intermediate network devices from dropping connections.
     *
     * param int|float $interval Interval in seconds (0 to disable)
     * @return DefaultBuilder self
     */
    public function withConnectionHeartbeatInterval($interval) { }

    /**
     * @return mixed
     */
    public function withPlaintextAuthenticator($username, $password) { }

    /**
     * @return mixed
     */
    public function withGssapiAuthenticator($service, $principal) { }

    /**
     * @return mixed
     */
    public function withGraphOptions($options) { }

}
