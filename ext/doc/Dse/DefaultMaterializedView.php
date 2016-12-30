<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a materialized view
 */
final class DefaultMaterializedView extends MaterializedView  {

    /**
     * {@inheritDoc}
     * @return string Name of the view
     */
    public function name() { }

    /**
     * {@inheritDoc}
     * @param mixed $name
     * @return Cassandra\Value Value of an option by name
     */
    public function option($name) { }

    /**
     * {@inheritDoc}
     *
     *               view's options.
     * @return array A dictionary of `string` and `Cassandra\Value pairs of the
     */
    public function options() { }

    /**
     * {@inheritDoc}
     * @return string Table description or null
     */
    public function comment() { }

    /**
     * {@inheritDoc}
     * @return float Read repair chance
     */
    public function readRepairChance() { }

    /**
     * {@inheritDoc}
     * @return float Local read repair chance
     */
    public function localReadRepairChance() { }

    /**
     * {@inheritDoc}
     * @return int GC grace seconds
     */
    public function gcGraceSeconds() { }

    /**
     * {@inheritDoc}
     * @return string Caching options
     */
    public function caching() { }

    /**
     * {@inheritDoc}
     * @return float Bloom filter FP chance
     */
    public function bloomFilterFPChance() { }

    /**
     * {@inheritDoc}
     * @return int Memtable flush period in milliseconds
     */
    public function memtableFlushPeriodMs() { }

    /**
     * {@inheritDoc}
     * @return int Default TTL.
     */
    public function defaultTTL() { }

    /**
     * {@inheritDoc}
     * @return string Speculative retry.
     */
    public function speculativeRetry() { }

    /**
     * {@inheritDoc}
     * @return int Index interval
     */
    public function indexInterval() { }

    /**
     * {@inheritDoc}
     * @return string Compaction strategy class name
     */
    public function compactionStrategyClassName() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Map Compaction strategy options
     */
    public function compactionStrategyOptions() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Map Compression parameters
     */
    public function compressionParameters() { }

    /**
     * {@inheritDoc}
     * @return boolean Value of `populate_io_cache_on_flush` or null
     */
    public function populateIOCacheOnFlush() { }

    /**
     * {@inheritDoc}
     * @return boolean Value of `replicate_on_write` or null
     */
    public function replicateOnWrite() { }

    /**
     * {@inheritDoc}
     * @return int Value of `max_index_interval` or null
     */
    public function maxIndexInterval() { }

    /**
     * {@inheritDoc}
     * @return int Value of `min_index_interval` or null
     */
    public function minIndexInterval() { }

    /**
     * {@inheritDoc}
     * @param string $name Name of the column
     * @return Cassandra\Column Column instance
     */
    public function column($name) { }

    /**
     * {@inheritDoc}
     * @return array A list of `Cassandra\Column` instances
     */
    public function columns() { }

    /**
     * {@inheritDoc}
     * @return array A list of of `Cassandra\Column` instances
     */
    public function partitionKey() { }

    /**
     * {@inheritDoc}
     * @return array A list of of `Cassandra\Column` instances
     */
    public function primaryKey() { }

    /**
     * {@inheritDoc}
     * @return array A list of of `Cassandra\Column` instances
     */
    public function clusteringKey() { }

    /**
     * {@inheritDoc}
     * @return array A list of cluster column orders ('asc' and 'desc')
     */
    public function clusteringOrder() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Table Base table of the view
     */
    public function baseTable() { }

}