#!/bin/sh
DIR=`dirname "$0"`
php -d extension=dse.so -d extension_dir="$DIR/../modules" "$DIR/generate_doc.php" $DIR/..
