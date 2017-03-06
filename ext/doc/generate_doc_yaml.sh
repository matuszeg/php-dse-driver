#!/bin/sh
php -d  extension=dse.so -d extension_dir=./modules/ generate_doc_yaml.php .
