[![Build Status](https://travis-ci.org/scandyna/multidiagtools.svg?branch=master)](https://travis-ci.org/scandyna/multidiagtools)

# About Mdt

Mdt is set of libraries, based on [Qt5](https://www.qt.io/),
that should help (in some future) interacting with some common instruments.

## Libraries

Mdt provides several libraries, placed in the libs directory.
Each library has a name, like Application, and is placed in a subdirectoy of the same name as base.
To reduce depenencies, some of them are splitted
(for example Application_Core, which depends on Qt5Core, or Application_Widgets, which also depends on Qt5Widgets).

To see currently available libraries, please take a look at the
[API documentation](https://scandyna.github.io/multidiagtools-apidoc/html) .

# Get Mdt

Currently, only sources are available on [github](https://github.com/scandyna/multidiagtools/).

## Linux

At first, we have to install git.
On a Debian system:
```bash
sudo apt-get install git
```

Then, create a directory to put Mdt and cd to it,
for example:
```bash
mkdir -p ~/opt/mdt/src
cd ~/opt/mdt/src
git clone git://github.com/scandyna/multidiagtools.git
```


# Compile Mdt

Mdt uses [CMake](https://cmake.org) as build management system.
It also depend on a C++14 compliant compiler
(f.ex. allmost recent version [GCC](https://gcc.gnu.org) or [Clang](https://clang.llvm.org) ).
Some libraries, like Qt5 are also needed.
To make things a bit mor easy,
we assume to use GCC as compiler, and [make](https://www.gnu.org/software/make/) as build system.
(CMake is able to generate other toolchains, but this is not part of this documentation)

## Linux

This section was tested on a Ubuntu 16.04 system.

### Tools and other dependencies

Install tools:
```bash
sudo apt-get install cmake make g++
```

To install Mdt, one option is to generate basic Debian packages (covered later in this documentation).
For this, following dependencies are also needed:
```bash
sudo apt-get install dpkg dpkg-dev
```

#### Install Boost

Mdt uses some [Boost](http://www.boost.org) libraries.
Installing it on a Debian system looks like:
```bash
sudo apt-get install libboost-dev
```

Optionnally, documentation can also be installed:
```bash
sudo apt-get install libboost-doc
```

#### Install Qt5

Mdt needs a recent version of Qt5.

##### Install Qt5 provided by the distribution

If your distribution provides the required version, you can use that.
For example, on Ubuntu:
```bash
sudo apt-get install qtbase5-dev qtbase5-dev-tools libqt5gui5 libqt5network5 libqt5sql5 libqt5sql5-mysql libqt5sql5-psql libqt5sql5-sqlite libqt5test5 libqt5widgets5
```

Optionnally, documentation can also be installed:
```bash
sudo apt-get install qtbase5-doc qtbase5-doc-html qtbase5-examples
```

### Compile Mdt on Linux

At first, int the source tree, create a build directory and cd to it:
```bash
cd ~/opt/mdt/src/multidiagtools
mkdir -p build/release
cd build/release
```

To avoid specifying to many options, Mdt provides some cache files that set common flags.
For example, for a release build with gcc, use:
```bash
cmake -C ../../cmake/caches/ReleaseGcc.cmake ../../
```
It is also possible to specify the intallation prefix:
```bash
cmake -C ../../cmake/caches/ReleaseGcc.cmake -D CMAKE_INSTALL_PREFIX=~/opt/mdt/release ../../
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
make -j4
```

To run all tests:
```bash
make test
```

### Install Mdt on Linux

#### Install in a predefined location

To install the library in a non system place,
i.e. defined above with CMAKE_INSTALL_PREFIX,
the installation is:
```bash
make install
```

This method is not recommanded for a system wide installation.

#### Install system wide on Debian

Please note:
this method should only work if Mdt depends on Qt5 library
that is installed system wide (not specified in QT_PREFIX_PATH).

This section will briefly describe how to generate packages.
Example to create Debian packages:
```bash
rm *.deb
make package_debian
```
To install all generated Debian packages, dpkg can be used:
```bash
sudo dpkg -i *.deb
```
To install only a subset of them, using dpkg can be tricky,
because it does not handles dependencies automatically.
It seems also that apt-get (and other tools based on it) can only work with a repository.
A possible way to create a local repository is:
Create a directory that will contain the Debian packages:
```bash
mkdir -p /home/you/opt/mdt/debs
```
Add a list file in /etc/apt/sources.list.d/ :
```bash
sudo echo "deb [trusted=yes] file:///home/you/opt/mdt/debs/ ./" > /etc/apt/sources.list.d/you_mdt_local.list
```
Copy the generated Debian packages and the package index file:
```bash
cp *.deb Packages.gz /home/you/opt/mdt/debs/
```
Update packages database to take the new local packages in account:
```bash
sudo apt-get update
```
Now, the packages can be installed with apt-get, Synaptic, etc..
Please note that generated packages are not fully compliant to Debian policy.
The dependencies are currently not complete.
For example, Sql-dev depends on qt5base5-dev, but the this is missing in the generated package.
For details on creating good Debian packages, see the [Debian packaging documentation](https://wiki.debian.org/Packaging)

CPack can generate other packages.
To have a list of packages generators that cpack has on your platform, use:
```bash
cpack --help
```
Please note that some generators needs some informations that current distribution does not provide.

