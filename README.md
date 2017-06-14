# About Mdt

Mdt is a library that should help (in some future) interacting with some common instruments.
But, today it's just a set of C++ classes, and nothing is complete. Mdt is based on Qt5, but some part of code is platform dependent.
The branch master is just very old, and work progress only in experimental branch.

## Libraries

### mdtcsv

Provides some helper classes to read and write CSV (file or string in memory).
A effort was made to follow most of the rules described in [CSV-1203](http://mastpoint.com/csv-1203) specifications,
but with some tolerances (for example, empty fields are allowed).
Internally, [Boost.Spirit V2](http://www.boost.org/doc/libs/1_60_0/libs/spirit/doc/html/index.html) is used.
Some work was also done to support QString, and the ability to read or write line by line.

# Using Mdt

## Linux

### Install dependencies

TODO:
 - A) all dependencies here, with comments
 - B) dependencies per sections (see below)

CMake
C++14 compliant compiler
dpkg-dev ?
Packet générique Debian, build-essentials-xx-yy-

Ubuntu (tested on ...)
apt-get ....

### Get Mdt

TODO: put depenendcies to get here.

Currently, only sources are available on github.

```bash
git clone git://github.com/scandyna/multidiagtools.git
```

### Compile Mdt

TODO: put dependnecies to build here

Mdt uses CMake as build tool.

At first, create a build directory and cd to it:
```bash
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
cmake -C ../../cmake/caches/ReleaseGcc.cmake -D CMAKE_INSTALL_PREFIX=~/opt/mdt ../../
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
make -j4
```

To run all tests:
```bash
make test
```

To install the library in a non system place,
i.e. defined above with CMAKE_INSTALL_PREFIX,
the installation is:
To run all tests:
```bash
make install
```

Above method is not recommandet for a system wide installation.
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
For details on creating good Debian packages, see https://wiki.debian.org/Packaging

CPack can generate other packages.
To have a list of packages generators that cpack has on your platform, use:
```bash
cpack --help
```
Please note that some generators needs some informations that current distribution does not provide.

### Use Mdt in your project

TODO: put dependencies to use here, OR see above...

Take a simple HelloWorld example.
Source file HelloWorld.cpp could be this:
```
#include <Mdt/ItemModel/VariantTableModel.h>
#include <QTableView>
#include <QApplication>

using namespace Mdt::ItemModel;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  VariantTableModel model;
  QTableView view;

  view.setModel(&model);
  view.show();
  model.populate(3, 2);

  return app.exec();
}
```

Write a minimal CMakeLists.txt file:
```
cmake_minimum_required(VERSION 3.3)

project(HelloWorld VERSION 0.0.1)

find_package(Qt5 COMPONENTS Widgets)
find_package(Mdt0 COMPONENTS ItemModel)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(helloworld HelloWorld.cpp)
target_link_libraries(helloworld Qt5::Widgets)
target_link_libraries(helloworld Mdt0::ItemModel)

```

Create a build directory and go to it:
```bash
mkdir -p build/debug
cd build/debug
```

If Mdt was installed in system standard way (for example using Debian packages),
following should be enouth:
```bash
cmake ../../
```

If Mdt was installed in a other place, cmake must know it:
```bash
cmake -D CMAKE_PREFIX_PATH=~/opt/mdt ../../
```


Windows

Note: this section is not up to date.

The library was tested on Windows XP and Wine. I use MinGW, so I will explain how to install and build all with it. The needed programs are:

* [CMake](http://www.cmake.org) 
* [MinGW](http://www.mingw.org)
  *  The simplest way is to use the packaged version from TDM-GCC project. I switched to TDM-GCC because I had trouble with MinGW packages. With mingw-get-inst-20120426's integrated catalog, gdb does not work, and online updated catalog provides GCC 4.7, witch cannot run Qt 4.8 binary distributed library. TDM installs gcc 4.6, and all worked fine for me on Windows XP 32bits.
*  [Qt5](//http://www.qt.io/download/)
  *  Built version of Qt4 library are available. For example: Latest version Note: during install, a warning will be displayed, saying that winAPIversion is wrong. In my case, this was not a problem, all works fine (on native system).
*  [NSIS](http://nsis.sourceforge.net/Download) (Optional, just to generate installer)

For each install, the simplest way is to keep default locations, else some work will be needed at next step.
Now, we have to edit a file: env-mingw.bat. We must specify the path to Qt4 directory:

```
set QTDIR=C:\Qt\4.8.2
Once done, cd to source tree, and run:
configure.bat
make clean
make
```

Note: on some version of MinGW, make alias is not made. In this case, the command becomes mingw32-make.

Install
=======

Note: this section is not up to date.

The best way to install is to generate packages, and install it. To create theses packages, run:

```
cpack .
```

Translations
============

Note: this section is not up to date.

The library use Qt's translation system. For a introduction, a tutorial is available on Qt's documentation.
In the translations subdirectory of source tree, some qmake project files are available.

To generate the .ts files, use the lupdate tool, for example:

```
lupdate translations/mdtutils.pro
```

To work on translations, use the linguist:

```
linguist translations/mdtutils_fr.ts
```

Once done, we have to generate the .qm files. For this, a rule was added in CMake project file. Simply run:

```
make qm
```
Note: if you just generated a .ts file, you need to run the configure script, else the new .ts file will not be considered.

