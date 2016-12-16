<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Cluster;

/**
 * Used to configure and construct a Dse\Cluster object.
 */
abstract class Builder implements Cassandra\Cluster\Builder {

    /**
     * Enable authentication with a plaintext username and password. This is for use
     * with DSE's builtin authentication and LDAP authentication.
     *
     * param string $username The username to use with DSE's plaintext authentication
     * param string $password The password to use with DSE's plaintext authentication
     * @return null
     */
    public abstract function withPlaintextAuthenticator($username, $password);

    /**
     * Enable authentication for use with Kerberos. By default the driver uses the connecting host's
     * IP address to construct the full service name (e.g. <service>@<IP address>). To use the host's
     * DNS name enable hostname resolution using Dse\Cluster\Builder::withHostnameResolution().
     *
     * @see Dse\Cluster\Builder::withHostnameResolution()
     *
     * param string $service The name of the Kerberos service
     * param string $principal The principal whose credentials are used to authenticate. If not provided then the first
     *                         principal in the Kerberos ticket cache will be used.
     * @return null
     */
    public abstract function withGssapiAuthenticator($service, $principal);

    /**
     * @return mixed
     */
    public abstract function withGraphOptions($options);

    /**
     * @return mixed
     */
    public abstract function build();

    /**
     * @return mixed
     */
    public abstract function withDefaultConsistency($consistency);

    /**
     * @return mixed
     */
    public abstract function withDefaultPageSize($pageSize);

    /**
     * @return mixed
     */
    public abstract function withDefaultTimeout($timeout);

    /**
     * @return mixed
     */
    public abstract function withContactPoints($host);

    /**
     * @return mixed
     */
    public abstract function withPort($port);

    /**
     * @return mixed
     */
    public abstract function withRoundRobinLoadBalancingPolicy();

    /**
     * @return mixed
     */
    public abstract function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies);

    /**
     * @return mixed
     */
    public abstract function withBlackListHosts($hosts);

    /**
     * @return mixed
     */
    public abstract function withWhiteListHosts($hosts);

    /**
     * @return mixed
     */
    public abstract function withBlackListDCs($dcs);

    /**
     * @return mixed
     */
    public abstract function withWhiteListDCs($dcs);

    /**
     * @return mixed
     */
    public abstract function withTokenAwareRouting($enabled);

    /**
     * @return mixed
     */
    public abstract function withCredentials($username, $password);

    /**
     * @return mixed
     */
    public abstract function withConnectTimeout($timeout);

    /**
     * @return mixed
     */
    public abstract function withRequestTimeout($timeout);

    /**
     * @return mixed
     */
    public abstract function withSSL($options);

    /**
     * @return mixed
     */
    public abstract function withPersistentSessions($enabled);

    /**
     * @return mixed
     */
    public abstract function withProtocolVersion($version);

    /**
     * @return mixed
     */
    public abstract function withIOThreads($count);

    /**
     * @return mixed
     */
    public abstract function withConnectionsPerHost($core, $max);

    /**
     * @return mixed
     */
    public abstract function withReconnectInterval($interval);

    /**
     * @return mixed
     */
    public abstract function withLatencyAwareRouting($enabled);

    /**
     * @return mixed
     */
    public abstract function withTCPNodelay($enabled);

    /**
     * @return mixed
     */
    public abstract function withTCPKeepalive($delay);

    /**
     * @return mixed
     */
    public abstract function withRetryPolicy($policy);

    /**
     * @return mixed
     */
    public abstract function withTimestampGenerator($generator);

    /**
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
     * param bool $enabled True to use hostname resolution; false otherwise.
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
     * param bool $enabled True to enable randomized contact points; false otherwise.
     * @return Builder self
     */
    public abstract function withRandomizedContactPoints($enabled);

    /**
     * Specify interval in seconds that the driver should wait before attempting
     * to send heartbeat messages and control the amount of time the connection
     * must be idle before sending heartbeat messages. This is useful for
     * preventing intermediate network devices from dropping connections.
     *
     * param int|float $interval Interval in seconds (0 to disable)
     * @return Builder self
     */
    public abstract function withConnectionHeartbeatInterval($interval);

}
