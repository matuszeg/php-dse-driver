<?php

/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

namespace Dse\Graph\Options;

/**
 */
final class Builder {

    /**
     * @return mixed
     */
    public function build() { }

    /**
     * @return mixed
     */
    public function withGraphLanguage($language) { }

    /**
     * @return mixed
     */
    public function withGraphSource($source) { }

    /**
     * @return mixed
     */
    public function withGraphName($name) { }

    /**
     * @return mixed
     */
    public function withReadConsistency($consistency) { }

    /**
     * @return mixed
     */
    public function withWriteConsistency($consistency) { }

    /**
     * @return mixed
     */
    public function withRequestTimeout($request_timeout) { }

}
