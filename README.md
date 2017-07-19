# About Mdt

Mdt is a library that should help (in some future) interacting with some common instruments.
But, today it's just a set of C++ classes, and nothing is complete. Mdt is based on Qt5, but some part of code is platform dependent.
The branch master is just very old, and work progress only in experimental branch.

## Licnce

## Libraries

### PlainText

Provides some helper classes to read and write plain text (file or string in memory).

Currently, the major part of this library is support for CSV.
A effort was made to follow most of the rules described in [CSV-1203](http://mastpoint.com/csv-1203) specifications,
but with some tolerances (for example, empty fields are allowed).
Internally, [Boost.Spirit V2](http://www.boost.org/doc/libs/1_60_0/libs/spirit/doc/html/index.html) is used.
Some work was also done to support QString, and the ability to read or write line by line.

# Get Mdt

Currently, only sources are available on github.

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

## Windows

TODO: to be documented

# Compile Mdt

Mdt uses CMake as build management system.
It also depend on a C++14 compliant compiler
(f.ex. allmost recent version GCC or Clang).
Some libraries, like Qt5 are also needed.
To make things a bit mor easy,
we assume to use GCC as compiler, and make as build system.
(CMake is able to generate other toolchains, but this is not part of this documentation)

## Linux

This section was tested on a Ubuntu 16.04 system.

### Tools and other dependencies

Install tools:
```bash
sudo apt-get install cmake make g++
```

Install libraries:
```bash
sudo apt-get install qtbase5-dev qtbase5-dev-tools libqt5gui5 libqt5network5 libqt5sql5 libqt5sql5-mysql libqt5sql5-psql libqt5sql5-sqlite libqt5test5 libqt5widgets5
```

Optionnally, documentation can also be installed:
```bash
sudo apt-get install qtbase5-doc qtbase5-doc-html qtbase5-examples
```

To install Mdt, one option is to generate basic Debian packages.
For this, following dependencies are also needed:
```bash
sudo apt-get install dpkg dpkg-dev
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

### Install Mdt on Linux

To install the library in a non system place,
i.e. defined above with CMAKE_INSTALL_PREFIX,
the installation is:
```bash
make install
```

Above method is not recommanded for a system wide installation.
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

## Windows

### Common tools

Following tools are needed to build and install Mdt on a native Windows system,
as well as cross-compile it on a Linux machine.

#### Dependency Walker

A optionnal, but helpfull tool, [Dependency Walker](http://www.dependencywalker.com/)
Note: choose a 32bit version (x86 , not x64) to be able to execute it with Wine.

### Cross compile Mdt for Windows on a Linux machine

My first attempt was to install Wine (then WineHQ)
and all tools and libraries on it, but this did not work for me.

Here we will use MXE to do cross-compilation.
You probably will use WineHQ to do some tests, in which case,
please remeber that WineHQ is possibly not able to run 64 bit executables.

#### Install WineHQ

WineHQ will be needed later to run the unit tests.
It can also be usefull to use, for example, dependency Walker,
or to check if your application works.

Please also note, for the nexte sections, that WineHQ probably not support 64 bit executable.

Using Ubuntu 16.04, I personally installed WineHQ successfully
by following the instructions avaiable on [WineHQ](https://www.winehq.org)
website, in the Download section.
Note: while running winecfg, I had to enable the option that emulates a desktop,
for some tools.
This is described here: [https://doc.ubuntu-fr.org/wine](https://doc.ubuntu-fr.org/wine)

#### Install MXE

MXE is a powerfull cross-compiler suite that will build all that we need.
Take a look at [MXE](http://mxe.cc) site for details.
As stated in the documentation, once MXE is built, it is not relocatable.
To reuse it for your own project, install it somwhere else than in Mdt source tree.
In my case, I choosed ~/opt/build/cross as base:
```bash
mkdir -p ~/opt/build/cross
```

MXE has a good step by step [Tutorial](http://mxe.cc/#tutorial),
simply follow it to install dependencies for your platform.

To make a 32 bit and 64 bit cross compiler that support boost, Qt5 and qwt:
```bash
cd ~/opt/build/cross
git clone https://github.com/mxe/mxe.git
cd mxe
make MXE_TARGETS='i686-w64-mingw32.shared.posix x86_64-w64-mingw32.shared.posix' boost qt5 qwt -j4
```

As described in the documentation, MXE will get and build the dependencies of the libraries we just specified.
The compilation will take some while.

Now go back to Mdt source tree.
Create a directory to build a Windows 32 bit version:
```bash
mkdir -p build/cross/win32/release
cd build/cross/win32
```

Finaly, write a script that will set environnment variable,
and unset some others as recommanded by MXE, and call the appropriate cmake wrapper.
Note: you should put the script in build/win32 , not build/win32/release ,
else you have the risk to delete it when doing a rm -r *

cmake-win32:
```
#/bin/sh
export PATH=~/opt/build/cross/mxe/usr/bin:$PATH
unset `env | grep -vi '^EDITOR=\|^HOME=\|^LANG=\|MXE\|^PATH=' | grep -vi 'PKG_CONFIG\|PROXY\|^PS1=\|^TERM=' | cut -d '=' -f1 | tr '\n' ' '`
i686-w64-mingw32.shared.posix-cmake "$@"
```

Make the script executable and check that it works:
```bash
chmod u+x cmake-win32
cd release
../cmake-win32 --version
```

Here is the equivalent script to target 64 bit buid, cmake-win64:
```
#/bin/sh
export PATH=~/opt/build/cross/mxe/usr/bin:$PATH
unset `env | grep -vi '^EDITOR=\|^HOME=\|^LANG=\|MXE\|^PATH=' | grep -vi 'PKG_CONFIG\|PROXY\|^PS1=\|^TERM=' | cut -d '=' -f1 | tr '\n' ' '`
x86_64-w64-mingw32.shared.posix-cmake "$@"
```

#### Cross compile Mdt for Windows

Mdt uses CMake as build tool.
Here we will use the script we created,
that calls the MXE CMake wrapper.

Make shure you allready are in the correct build directory
(f.ex. build/cross/win32/release)

To avoid specifying to many options, Mdt provides some cache files that set common flags.
For example, for a release build with gcc, use:
```bash
../cmake-win32 -C ../../../../cmake/caches/ReleaseMxe.cmake ../../../../
```
It is also possible to specify the intallation prefix:
```bash
../cmake-win32 -C ../../../../cmake/caches/ReleaseMxe.cmake -D CMAKE_INSTALL_PREFIX=~/opt/mdt/win32 ../../../../
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
make -j4
```

To run all tests:
```bash
make test
```
Note that to run the tests, CMake will run wine.

To install the library in a non system place,
i.e. defined above with CMAKE_INSTALL_PREFIX,
the installation command is:
```bash
make install
```

### Compile Mdt on Windows

#### Additionnal tools and dependencies

##### CMake

##### MinGW

##### Boost

##### MSYS2 ?

##### ICU ?

##### Qt5

##### Qwt

#### Compile Mdt


# Use Mdt in your project

## A HelloWorld example

Take a simple HelloWorld example.
Source file HelloWorld.cpp could be this:
```cpp
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
```cmake
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

## Build your project on Linux

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

## Cross compile your project for Windows on a Linux machine

## Build your project on Windows




# Obselete documentation

Following sections are obselete.
They are keeped here to later document
how to use Mdt on a native Windows system.

#### CMake

Install CMake available from [CMake](https://cmake.org/) website.
Again, choose a 32bit executable (f.ex. win32-x86 installer).
Using a msi installer is fine.
To install CMake, in the directory where the installer file lives:
```bash
wine msiexec /i cmake-x.y.z-win32-x86.msi
```
Note: when the wizard ask about adding CMake to the system PATH,
its recommanded to add it, else you will have additionnal work
to create a developpement environnment (described later).
I personally choosed "Add CMake to the system PATH for the current user"
For other options, I personally choosed the default ones.

Try if CMake works:
```bash
wine cmake --version
```

#### MinGW (compiler suite including GCC)

Qt binary package for gcc (mingw) is shipped with the compiler,
and batch file that provides a developpment environnment.
In my case, the provided compiler was to old.
Mdt can only be compiled with a fully C++14 compliant compiler.

The recommanded solution is to use [TDM-GCC](http://tdm-gcc.tdragon.net)
In the Download section, choose a 32 bit installer.
Run the installer. In my case:
```bash
wine tdm-gcc-5.1.0-3.exe
```
The default installation path should be fine (C:\TDM-GCC-32)
This path will be reused later when creating the developpement environnment batch file.
By default, the installer should have C++ suport enabled. If not, enable it.
I personally decided to not add TDM-GCC to the PATH.

#### MSYS

Note: currently, this section was abandoned.

In my case, I failed to install MSYS2, because the installer crashed at 66% of the installation.
This is why using MSYS.
And: how must MSYS be installed ?
Go to the [Sourceforg MinGW project](https://sourceforge.net/projects/mingw/).


#### Qt5

Note: currently, this section was abandoned.

The Qt Company ships some binary distributions of Qt5,
available here: [https://www.qt.io/download/](https://www.qt.io/download/)
The time I wrote this help, only a MinGW with gcc 4.9 was available.
Because gcc ABI changed between gcc 4.9 and gcc 5,
compiling Mdt was not possible by using the Qt5 binary package.
In this case, sorry, we have to compile Qt5.

To have a better idea about how to compile Qt,
you should have a look at the official documentation
on [http://doc.qt.io/](http://doc.qt.io/) in the "Getting Started Guides" section.

At first, we have to get and compile the ICU library.
Qt provides some [documentation](https://wiki.qt.io/Compiling-ICU-with-MinGW) on how to achieve that.
Go to the [ICU Download](http://site.icu-project.org/download) page
and choose an ICU4C source package.
In my case, it was "icu4c-59_1-src.zip"
Extract the archive somewhere and cd to it.


Go to the [Download](https://www.qt.io/download) page,
Choose Desktop applications, then your licence.
Then, while you are not on Windows now, choose "View All Downloads"
Download the zip archive provided for Windows.
In my case, it was "qt-everywhere-opensource-src-5.9.1.zip"
Extract the archive somewhere and cd to it.



TODO use notepad to create mingw32-env.bat



You must choose a 32bit version, that was compiled with gcc (not VS).
Personally, the only package I found, was a offline installer:
Qt 5.6.2 for Windows 32-bit (MinGW 4.9.2)
This is a executable thatcan be run like this:
```bash
wine qt-opensource-windows-x86-mingwxyz-x.y.z.exe
```
You should choose the default proposed installation directory.

#### Boost

Boost libraries are available here: [www.boost.org](www.boost.org)
For complete installation documentation, see [Getting Started](www.boost.org/more/getting_started/index.html)
For me, the following worked, without having to set any environnment variable:
 - Create a boost directory in ~/.wine/drive_c/Program Files/
 - Download a Windows archive from [Boost](www.boost.org) , current release
 - Extract the archive to ~/.wine/drive_c/Program Files/boost/
My resulting directory path is: ~/.wine/drive_c/Program Files/boost/boost_1_64_0/

### Compile Mdt

Note: currently, this section was abandoned.

Mdt uses CMake as build tool.

At first, create a build directory and cd to it:
```bash
mkdir -p build/win32/release
cd build/win32/release
```



In my case, the provided compiler was to old.
Mdt can only be compiled with a fully C++14 compliant compiler.



Qt binary package for gcc (mingw) is shipped with the compiler,
and batch file that provides a developpment environnment.

For ease of use, you can copy this file to the build directory.
For example, in my case, with Qt5.6.2:
```bash
cp ~/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/qtenv2.bat mingw32-env.bat
```
Also edit the file:
 - If you had to install mingw (TDM-GCC) separatly, replace the path to mingw Tools, but let the path to Qt as is
   In my case, the result is: set PATH=C:\Qt\Qt5.6.2\5.6\mingw49_32\bin;C:\TDM-GCC-32\bin\;%PATH%
 - Remove that last line that will dc to C:\Qt\Qt5.x.y.z\y.z\mingwxy_ab

Now, run a DOS shell:
```bash
wine cmd
```
You should now be in a DOS shell.
In my case, using Konsole, I just see a Microsoft Windows version,
and my current path is on Z:\ root.

Build the developpement environnment:
```bash
mingw32-env.bat
```

You can check if PATH contains all that is requird:
```bash
echo %PATH%
```
You should have at least 2 entry for Qt, all default Windows paths and CMake.
Check gcc C++ compiler version:
```bash
g++ --version
```
The version of g++ should be displayed. Check that g++ version >= 5

Now compile Mdt.
To avoid specifying to many options, Mdt provides some cache files that set common flags.
For example, for a release build with gcc, use:
```bash
cmake -C ../../../cmake/caches/ReleaseGcc.cmake ../../../ -G "MinGW Makefiles"
```
It is also possible to specify the intallation prefix:
```bash
cmake -C ../../../cmake/caches/ReleaseGcc.cmake -D CMAKE_INSTALL_PREFIX=/Mdt ../../../ -G "MinGW Makefiles"
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
mingw32-make -j4
```

To run all tests:
```bash
make test
```

## Windows

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

