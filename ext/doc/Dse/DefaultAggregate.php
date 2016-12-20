<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of an aggregate
 */
final class DefaultAggregate implements Aggregate {

    /**
     * {@inheritDoc}
     * @return string Full name of the aggregate including name and types
     */
    public function name() { }

    /**
     * {@inheritDoc}
     * @return string Simple name of the aggregate
     */
    public function simpleName() { }

    /**
     * {@inheritDoc}
     * @return array Argument types of the aggregate
     */
    public function argumentTypes() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Function State public function of the aggregate
     */
    public function stateFunction() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Function Final public function of the aggregate
     */
    public function finalFunction() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Value Initial condition of the aggregate
     */
    public function initialCondition() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Type State type of the aggregate
     */
    public function stateType() { }

    /**
     * {@inheritDoc}
     * @return Cassandra\Type Return type of the aggregate
     */
    public function returnType() { }

    /**
     * {@inheritDoc}
     * @return string Signature of the aggregate (same as name())
     */
    public function signature() { }

}
