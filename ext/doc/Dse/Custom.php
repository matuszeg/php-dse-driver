<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse;

/**
 * An interface for representing custom values.
 */
abstract class Custom implements Value {

    /**
     * The type of this value.
     * @return Type\Custom
     */
    public abstract function type();

}
