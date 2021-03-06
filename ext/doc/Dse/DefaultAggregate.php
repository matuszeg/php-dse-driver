<?php

/*
  Copyright (c) 2017 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of an aggregate
 */
final class DefaultAggregate implements Aggregate {

    /**
     * Returns the full name of the aggregate
     *
     * @return string Full name of the aggregate including name and types
     */
    public function name() { }

    /**
     * Returns the simple name of the aggregate
     *
     * @return string Simple name of the aggregate
     */
    public function simpleName() { }

    /**
     * Returns the argument types of the aggregate
     *
     * @return array Argument types of the aggregate
     */
    public function argumentTypes() { }

    /**
     * Returns the state function of the aggregate
     *
     * @return Function State public function of the aggregate
     */
    public function stateFunction() { }

    /**
     * Returns the final function of the aggregate
     *
     * @return Function Final public function of the aggregate
     */
    public function finalFunction() { }

    /**
     * Returns the initial condition of the aggregate
     *
     * @return Value Initial condition of the aggregate
     */
    public function initialCondition() { }

    /**
     * Returns the state type of the aggregate
     *
     * @return Type State type of the aggregate
     */
    public function stateType() { }

    /**
     * Returns the return type of the aggregate
     *
     * @return Type Return type of the aggregate
     */
    public function returnType() { }

    /**
     * Returns the signature of the aggregate
     *
     * @return string Signature of the aggregate (same as name())
     */
    public function signature() { }

}
