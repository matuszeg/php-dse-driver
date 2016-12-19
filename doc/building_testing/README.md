# Building and Testing

## Introduction
The PHP DSE driver is implemented as a native PHP extension that heavily leverages the C++ DSE driver.
Since there are fundamental differences between PHP5 and PHP7 native api's, it's important to build and
test any changes on both major versions. The driver is known to work with PHP 5.6.x and PHP 7.0.x as
of this writing.
 
## Supported Platforms
The driver is known to work on CentOS/RHEL 6/7, Mac OS X 10.8/10.9/10.10/10.11
(Moutain Lion, Mavericks, Yosemite, and El Capitan), Ubuntu 12.04/14.04 LTS, and
Windows 7 SP1 and above.

It has been built using GCC 4.1.2+, Clang 3.4+, and MSVC 2010/2012/2013/2015.

## Dependencies
- [CMake](http://www.cmake.org)

## Initial Environment Setup

### MacOS
```
brew install php70 --with-thread-safety --with-debug --with-pear
brew install php56 --with-thread-safety --with-debug --with-pear
brew install php70-yaml
brew install libuv
brew install gmp
brew install openssl
brew install libtool
brew install libev
brew install doxygen
brew install autoconf
brew install automake
```

### Linux
TBD

### Windows
TBD

## Getting the driver source
Clone the repository from GitHub and initialize submodules (the C++ DSE driver source is a submodule):
```
git clone git@github.com:datastax/php-driver-dse-private.git
git submodule update --init
```
## Build Instructions

### Best practices
Although not strictly required, you shouold follow these best practices:

* build the driver into separate build output directories for each version of PHP.
* run the extension from the build output directory.

### MacOS
To make it easy to keep these build output directories separate and switch the "active" version
of PHP, we introduce the following bash functions; convert as desired to the shell of your choice:

```
function php7() {
  local PHPCURVER=`php --version | head -1 | awk '{print $2}' | awk 'BEGIN { FS="." } {print $1}'`
  if [ "$PHPCURVER" != "7" ] ; then
    brew unlink php56
    brew link php70
    rm ~/php-driver-dse-private/build
    (cd ~/php-driver-dse-private; mkdir -p build70; ln -s build70 build)
  else
    echo "Already on php7"
  fi
}

function php5() {
  local PHPCURVER=`php --version | head -1 | awk '{print $2}' | awk 'BEGIN { FS="." } {print $1}'`
  if [ "$PHPCURVER" != "5" ] ; then
    brew unlink php70
    brew link php56
    rm ~/php-driver-dse-private/build
    (cd ~/php-driver-dse-private; mkdir -p build56; ln -s build56 build)
  else
    echo "Already on php5"
  fi
}
```

The functions assume the following:
* the repo is cloned into your home directory
* the build output directory for PHP 5 is named `build56`
* the build output directory for PHP 7 is named `build70`

These functions create a symlink named `build` to point to the build directory that corresponds to the active php.
This allows you to consistently deal with the one directory (semantically) regardless of php version.

At this point, build the driver with the same instructions as Linux.

#### Ignoring build artifacts in Git
Since this setup yields two new build directories (build56 and build70), you'll want to exclude these from git. Since
these build directories are specific to the setup described earlier, it may not be appropriate to exclude these in
.gitignore and committed. Instead, add them to you local excludes:

```
cat >> .git/info/exclude << EOF
build56
build70
EOF
```

### Linux

To build the driver into a `build` directory in the repo, you must first generate the configure script for the
version of php you are using:
```
cd ~/php-driver-dse-private/ext
phpize
```

Then go to the build directory, run the configure script, and build the extension:
```
cd ~/php-driver-dse-private
mkdir -p build; cd build
CFLAGS='-g -O0 -Wdeclaration-after-statement -Wno-extended-offsetof' ../ext/configure  --prefix=/usr/local/ --exec-prefix=/usr/local/ --with-gmp=/usr/local --with-uv=/usr/local 
make -j8
```

If the build succeeds, the extension will be `modules/dse.so` under the build directory.

## Running
PHP will not know where your module is since it hasn't been installed or even registered in php.ini. To keep php
installations clean, create your own php.ini (say in your home directory) as follows:

```
extension_dir=<PHP DSE Driver Source Dir>/build/modules
extension=dse.so
date.timezone=America/Phoenix
cassandra.log=dse.log
cassandra.log_level=DEBUG
```
Be sure to update the reference to the base directory of your cloned driver repo above.

Set the PHPRC environment variable so that php invocations (as well as other tools like phpunit) will use your .ini
file by default.

```
export PHPRC=$HOME/php.ini 
```

Verify that the module loads like this:

```
php -i | grep -A 5 Dse
```

## Testing
Unit tests and integration tests are written for phpunit. Download and install composer to set things up:

```
cd <PHP DSE Driver Source Dir>
curl -sS https://getcomposer.org/installer | php
php composer.phar install
php composer.phar dumpautoload
```

Run tests from the top of the repo:
```
# Run a subset of the unit tests
bin/phpunit --testsuite unit --filter Point

# Run integration tests
bin/phpunit --testsuite integration
```

## Document Generation
Because all of the api's of the driver are implemented in C, the public documentation lives in yaml files for each
class alongside the corresponding source files. When adding a new class, run the `ext/generate_doc_yaml.sh` shell script
to produce a new yaml file for the class. Fill in that yaml file with documentation for the class and its methods.

Whenever you change the yaml file for a class, run `ext/generate_doc.sh` to generate stub php files with the documentation
from the yaml files.

Once the PHP files are generated, run documentor to produce proper html documentation:
```
~/documentor/bin/documentor --only-version=LOCAL ~/php-driver-dse-private
```

The `--only-version=LOCAL` argument tells Documentor to action on your local source tree rather than the tags/shas of
past committed versions of the driver. The html tree is written out in the `docs` directory of the repo. View the 
generated docs in a browser. Continue refining docs in the yaml files, running `generate_doc.sh`, running Documentor,
and viewing the final results in a browser until you're satisfied.