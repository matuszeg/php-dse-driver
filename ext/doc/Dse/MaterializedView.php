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
abstract class MaterializedView implements Table {

    /**
     * Returns the base table of the view
     * @return Dse\Table Base table of the view
     */
    public abstract function baseTable();

    /**
     * Returns the name of this table
     * @return string Name of the table
     */
    public abstract function name();

    /**
     * Return a table's option by name
     * @param mixed $name
     * @return Dse\Value Value of an option by name
     */
    public abstract function option($name);

    /**
     * Returns all the table's options
     *               view's options.
     * @return array A dictionary of `string` and `Value` pairs of the
     */
    public abstract function options();

    /**
     * Description of the table, if any
     * @return string Table description or null
     */
    public abstract function comment();

    /**
     * Returns read repair chance
     * @return float Read repair chance
     */
    public abstract function readRepairChance();

    /**
     * Returns local read repair chance
     * @return float Local read repair chance
     */
    public abstract function localReadRepairChance();

    /**
     * Returns GC grace seconds
     * @return int GC grace seconds
     */
    public abstract function gcGraceSeconds();

    /**
     * Returns caching options
     * @return string Caching options
     */
    public abstract function caching();

    /**
     * Returns bloom filter FP chance
     * @return float Bloom filter FP chance
     */
    public abstract function bloomFilterFPChance();

    /**
     * Returns memtable flush period in milliseconds
     * @return int Memtable flush period in milliseconds
     */
    public abstract function memtableFlushPeriodMs();

    /**
     * Returns default TTL.
     * @return int Default TTL.
     */
    public abstract function defaultTTL();

    /**
     * Returns speculative retry.
     * @return string Speculative retry.
     */
    public abstract function speculativeRetry();

    /**
     * Returns index interval
     * @return int Index interval
     */
    public abstract function indexInterval();

    /**
     * Returns compaction strategy class name
     * @return string Compaction strategy class name
     */
    public abstract function compactionStrategyClassName();

    /**
     * Returns compaction strategy options
     * @return Dse\Map Compaction strategy options
     */
    public abstract function compactionStrategyOptions();

    /**
     * Returns compression parameters
     * @return Dse\Map Compression parameters
     */
    public abstract function compressionParameters();

    /**
     * Returns whether or not the `populate_io_cache_on_flush` is true
     * @return bool Value of `populate_io_cache_on_flush` or null
     */
    public abstract function populateIOCacheOnFlush();

    /**
     * Returns whether or not the `replicate_on_write` is true
     * @return bool Value of `replicate_on_write` or null
     */
    public abstract function replicateOnWrite();

    /**
     * Returns the value of `max_index_interval`
     * @return int Value of `max_index_interval` or null
     */
    public abstract function maxIndexInterval();

    /**
     * Returns the value of `min_index_interval`
     * @return int Value of `min_index_interval` or null
     */
    public abstract function minIndexInterval();

    /**
     * Returns column by name
     * @param string $name Name of the column
     * @return Dse\Column Column instance
     */
    public abstract function column($name);

    /**
     * Returns all columns in this table
     * @return array A list of `Dse\Column` instances
     */
    public abstract function columns();

    /**
     * Returns the partition key columns of the table
     * @return array A list of of `Dse\Column` instances
     */
    public abstract function partitionKey();

    /**
     * Returns both the partition and clustering key columns of the table
     * @return array A list of of `Dse\Column` instances
     */
    public abstract function primaryKey();

    /**
     * Returns the clustering key columns of the table
     * @return array A list of of `Dse\Column` instances
     */
    public abstract function clusteringKey();

    /**
     * @return array A list of cluster column orders ('asc' and 'desc')
     */
    public abstract function clusteringOrder();

}
