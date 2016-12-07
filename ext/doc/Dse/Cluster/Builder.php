<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Cluster;

/**
 */
abstract class Builder implements Cassandra\Cluster\Builder {

    /**
     * @param mixed $username
     * @param mixed $password
     * @return mixed
     */
    public abstract function withPlaintextAuthenticator($username, $password);

    /**
     * @param mixed $service
     * @param mixed $principal
     * @return mixed
     */
    public abstract function withGssapiAuthenticator($service, $principal);

    /**
     * @param mixed $language
     * @return mixed
     */
    public abstract function withGraphLanguage($language);

    /**
     * @param mixed $source
     * @return mixed
     */
    public abstract function withGraphSource($source);

    /**
     * @param mixed $name
     * @return mixed
     */
    public abstract function withGraphName($name);

    /**
     * @param mixed $consistency
     * @return mixed
     */
    public abstract function withGraphReadConsistency($consistency);

    /**
     * @param mixed $consistency
     * @return mixed
     */
    public abstract function withGraphWriteConsistency($consistency);

    /**
     * @param mixed $request_timeout
     * @return mixed
     */
    public abstract function withGraphRequestTimeout($request_timeout);

    /**
     * @return mixed
     */
    public abstract function build();

    /**
     * @param mixed $consistency
     * @return mixed
     */
    public abstract function withDefaultConsistency($consistency);

    /**
     * @param mixed $pageSize
     * @return mixed
     */
    public abstract function withDefaultPageSize($pageSize);

    /**
     * @param mixed $timeout
     * @return mixed
     */
    public abstract function withDefaultTimeout($timeout);

    /**
     * @param mixed $host
     * @return mixed
     */
    public abstract function withContactPoints($host);

    /**
     * @param mixed $port
     * @return mixed
     */
    public abstract function withPort($port);

    /**
     * @return mixed
     */
    public abstract function withRoundRobinLoadBalancingPolicy();

    /**
     * @param mixed $localDatacenter
     * @param mixed $hostPerRemoteDatacenter
     * @param mixed $useRemoteDatacenterForLocalConsistencies
     * @return mixed
     */
    public abstract function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies);

    /**
     * @param mixed $hosts
     * @return mixed
     */
    public abstract function withBlackListHosts($hosts);

    /**
     * @param mixed $hosts
     * @return mixed
     */
    public abstract function withWhiteListHosts($hosts);

    /**
     * @param mixed $dcs
     * @return mixed
     */
    public abstract function withBlackListDCs($dcs);

    /**
     * @param mixed $dcs
     * @return mixed
     */
    public abstract function withWhiteListDCs($dcs);

    /**
     * @param mixed $enabled
     * @return mixed
     */
    public abstract function withTokenAwareRouting($enabled);

    /**
     * @param mixed $username
     * @param mixed $password
     * @return mixed
     */
    public abstract function withCredentials($username, $password);

    /**
     * @param mixed $timeout
     * @return mixed
     */
    public abstract function withConnectTimeout($timeout);

    /**
     * @param mixed $timeout
     * @return mixed
     */
    public abstract function withRequestTimeout($timeout);

    /**
     * @param Cassandra\SSLOptions $options
     * @return mixed
     */
    public abstract function withSSL($options);

    /**
     * @param mixed $enabled
     * @return mixed
     */
    public abstract function withPersistentSessions($enabled);

    /**
     * @param mixed $version
     * @return mixed
     */
    public abstract function withProtocolVersion($version);

    /**
     * @param mixed $count
     * @return mixed
     */
    public abstract function withIOThreads($count);

    /**
     * @param mixed $core
     * @param mixed $max
     * @return mixed
     */
    public abstract function withConnectionsPerHost($core, $max);

    /**
     * @param mixed $interval
     * @return mixed
     */
    public abstract function withReconnectInterval($interval);

    /**
     * @param mixed $enabled
     * @return mixed
     */
    public abstract function withLatencyAwareRouting($enabled);

    /**
     * @param mixed $enabled
     * @return mixed
     */
    public abstract function withTCPNodelay($enabled);

    /**
     * @param mixed $delay
     * @return mixed
     */
    public abstract function withTCPKeepalive($delay);

    /**
     * @param Cassandra\RetryPolicy $policy
     * @return mixed
     */
    public abstract function withRetryPolicy($policy);

    /**
     * @param Cassandra\TimestampGenerator $generator
     * @return mixed
     */
    public abstract function withTimestampGenerator($generator);

    /**
     * @param mixed $enabled
     * @return mixed
     */
    public abstract function withSchemaMetadata($enabled);

    /**
     * Enables/Disables Hostname Resolution.
     *
     * If enabled the driver will resolve hostnames for IP addresses using
     * reverse IP lookup. This is useful for authentication (Kerberos) or
     * encryption SSL services that require a valid hostname for verification.
     *
     * @param bool $enabled True to use hostname resolution; false otherwise.
     * @return Builder self
     */
    public abstract function withHostnameResolution($enabled);

    /**
     * Enable/Disables Randomized Contact Points
     *
     * If enabled this allows the driver randomly use contact points in order
     * to evenly spread the load across the cluster and prevent
     * hotspots/load spikes during notifications (e.g. massive schema change).
     *
     * Note: This setting should only be disabled for debugging and testing.
     *
     * @param bool $enabled True to enable randomized contact points; false otherwise.
     * @return Builder self
     */
    public abstract function withRandomizedContactPoints($enabled);

    /**
     * Specify interval in seconds that the driver should wait before attempting
     * to send heartbeat messages and control the amount of time the connection
     * must be idle before sending heartbeat messages. This is useful for
     * preventing intermediate network devices from dropping connections.
     *
     * @param int|float $interval Interval in seconds (0 to disable)
     * @return Builder self
     */
    public abstract function withConnectionHeartbeatInterval($interval);

}
