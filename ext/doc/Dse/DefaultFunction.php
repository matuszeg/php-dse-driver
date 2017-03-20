<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a public function
 */
final class DefaultFunction implements Function_ {

    /**
     * {@inheritDoc}
     *
     * @return string Full name of the function including name and types
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string Simple name of the function
     */
    public function simpleName() { }

    /**
     * {@inheritDoc}
     *
     * @return array Arguments of the function
     */
    public function arguments() { }

    /**
     * {@inheritDoc}
     *
     * @return Type Return type of the function
     */
    public function returnType() { }

    /**
     * {@inheritDoc}
     *
     * @return string Signature of the function (same as name())
     */
    public function signature() { }

    /**
     * {@inheritDoc}
     *
     * @return string Language used by the function
     */
    public function language() { }

    /**
     * {@inheritDoc}
     *
     * @return string Body of the function
     */
    public function body() { }

    /**
     * {@inheritDoc}
     *
     * @return bool
     */
    public function isCalledOnNullInput() { }

}
