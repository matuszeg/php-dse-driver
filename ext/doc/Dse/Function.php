<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * A PHP representation of a function
 */
interface Function_ {

    /**
     * Returns the full name of the function
     * @return string Full name of the function including name and types
     */
    public function name();

    /**
     * Returns the simple name of the function
     * @return string Simple name of the function
     */
    public function simpleName();

    /**
     * Returns the arguments of the function
     * @return array Arguments of the function
     */
    public function arguments();

    /**
     * Returns the return type of the function
     * @return Dse\Type Return type of the function
     */
    public function returnType();

    /**
     * Returns the signature of the function
     * @return string Signature of the function (same as name())
     */
    public function signature();

    /**
     * Returns the lanuage of the function
     * @return string Language used by the function
     */
    public function language();

    /**
     * Returns the body of the function
     * @return string Body of the function
     */
    public function body();

    /**
     * Determines if a funciton is called when the value is null.
     * @return bool Returns whether the function is called when the input columns are null
     */
    public function isCalledOnNullInput();

}
