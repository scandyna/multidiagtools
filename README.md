Linux: [![Build Status](https://travis-ci.org/scandyna/multidiagtools.svg?branch=master)](https://travis-ci.org/scandyna/multidiagtools)
Windows: [![Build Status](https://ci.appveyor.com/api/projects/status/aov9mxmr5nnvuxkb?svg=true)](https://ci.appveyor.com/project/scandyna/multidiagtools)

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

## Windows

At first, we have to install [git](https://git-scm.com/).
Get a version that corresponds to your platform in the [Downloads section](https://git-scm.com/downloads).
While running the installer, I personally accepted default settings, mainly:
* "Windows explorer integration" options
* "Use Git from the Windows Command Prompt" which adds git to the PATH.
* "Use the OpenSSL library"
* "Checkout Windows-style, commit Unix-style line endings"

Then, create a directory to put Mdt and navigate to it,
then right-click and choose "Git Bash Here".
Finaly, clone the Mdt repository:
```bash
git clone git://github.com/scandyna/multidiagtools.git
```

# Compile Mdt

Mdt uses [CMake](https://cmake.org) as build management system.
It also depend on a C++14 compliant compiler
(f.ex. allmost recent version [GCC](https://gcc.gnu.org) or [Clang](https://clang.llvm.org) ).
Some libraries, like [Qt5](https://www.qt.io/) are also needed.
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

Some tools from DeployUtils have some runtime dependencies,
like objdump and patchelf:
```bash
sudo apt-get binutils patchelf
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

##### Install Qt5 if not provided by the distribution

It's also recommanded to have a look at the [documentation](http://doc.qt.io/)
in the "Getting Started Guides" section.
Maybe you will have to create a [Qt Acount](https://account.qt.io)

At first, install [requirements](http://doc.qt.io/qt-5/linux.html) .

Then, you can get Qt5 from the [Download section](https://www.qt.io/download/) .

Choose "Desktop & Mobile Application", then the licencing option.
The default proposed package should be fine.

We must make the installer executable and run it.
In my case, I have put the installer to ~/opt/qt/installers:
```bash
cd ~/opt/qt/installers
chmod u+x qt-unified-linux-x64-3.0.0-online.run
./qt-unified-linux-x64-3.0.0-online.run
```
Of course, the name of the installer can be different in your case.

Follow the wizzard.
In my case, I choosed to install Qt5 to ~/opt/qt/Qt5

### Compile Mdt on Linux

At first, init the source tree, create a build directory and cd to it:
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


To use a Qt5 library that is not installed in the default path,
we have also to specify it:
```bash
cmake -C ../../cmake/caches/ReleaseGcc.cmake -D QT_PREFIX_PATH=~/opt/qt/Qt5/5.9.1/gcc_64/ ../../
```

This is my personnal case to build Mdt in debug mode, and install it locally:
```bash
cmake -C ../../cmake/caches/DebugGcc.cmake -D QT_PREFIX_PATH=~/opt/qt/Qt5/5.9.1/gcc_64/ -D CMAKE_INSTALL_PREFIX=~/opt/mdt/debug ../../
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

## Windows

This section will suggest how to compile Mdt on a Windows machine,
as well as cross-compile it for Windows from a Linux machine.

### Common tools

#### Dependecies Walker

To find some informations about dll's, such as missing dependencies,
[Dependency Walker](http://www.dependencywalker.com/) can be helpful.

Note: on Linux, using Wine, probably only the 32 bit version will work
(choose x86, not x64).

### Compile Mdt on a windows machine

This section was tested on Windows 10 x86-64.

#### Tools and other dependencies

##### CMake

Mdt uses [CMake](https://cmake.org/) as build tool.
Choose a installer that matches your platform from the [Download](https://cmake.org/download/) section.
Here are some options that I choosed (all other I keeped default):
* "Add CMake to the system PATH for the current user"

#### Install Qt5 and Gcc

It is recommanded to have a look at the [documentation](https://doc.qt.io/) in the "Getting Started Guides" section. Myabe you will have to create a [Qt Acount](https://login.qt.io).

Qt5 can be downloaded from [here](https://www.qt.io/download/).
Choose "Desktop & Mobile Applications", then the licencing option.
Check that the proposed installer matches your platform and start the download.
In my case, a online installer was selected.

Note: while writing these lines, only 32 bit installers are available for MinGW.

Run the installer.
You probably will have to login with your acount created before.
It will be asked about the installation path, which we have to remember for a later step.
For the rest of this document, C:\Qt will be considered.
It's also possible to select components to install.
The default should be fine.

Once installation is done, a Qt entry should be added in the Start/Windows menu.
Bellow it, execute the shortcut called "Qt x.y.z for Desktop".
A window with a command prompt should pop-up.
From this prompt, all that is needed to compile Qt applications is in the PATH.

Check gcc version:
```bash
gcc --version
```
It should return a version at least 5.0.0 , in which case C++14 support is good.

Check Qt installation:
```bash
qmake -version
```
Check that the expected Qt library is used.

#### Install Boost

It is recommanded to have a look at the [documentation](http://www.boost.org/) in the "Getting stated" section.

We will install Boost libraries to the Windows Program Files.
When doing so, CMake will be able to find it automatically.

Create a directory named "boost" in C:\Program Files .

Get a Boost archive from the [Download section](http://www.boost.org/users/download/) and put it somewhere.

Extract the archive to C:\Program Files\boost .

#### Compile Mdt on Windows

For this section, it will be considered that the Mdt source tree is:
%HOMEPATH%\Documents\opt\Mdt\src\multidiagtools\

In the root of Mdt source tree, create a directory called "build", and a subdirectory called "release".

Open the Qt command prompt (Start/Windows menu -> Qt -> Qt x.y.z for Desktop.
Go to the created build directory:
```bash
cd %HOMEPATH%\Documents\opt\Mdt\src\multidiagtools\build\release
```

To avoid specifying to many options, Mdt provides some cache files that set common flags.
For example, for a release build with gcc and make, use:
```bash
cmake -C ..\..\cmake\caches\ReleaseGcc.cmake -G "MinGW Makefiles" ..\..\
```
It is also possible to specify the intallation prefix:
```bash
cmake -C ..\..\cmake\caches\ReleaseGcc.cmake -G "MinGW Makefiles" -D CMAKE_INSTALL_PREFIX="C:/Users/You/Documents/opt/Mdt/release" ..\..\
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
mingw32-make -j4
```

To run all tests:
```bash
mingw32-make test
```

During build directory initialization,
a script called mdtenv.bat was generated,
which setups a environment, like qtenv2.bat does.
It will set the PATH to include:
- Path to the executable directory of the compiler
- Path to the executable directory of the Qt5 library used to compile Mdt

To run a command prompt with this environment set,
you can create a shortcut that runs cmd.exe and calls this mdtenv.bat.
Example of options of that shortcut could be:
- Target: C:\Windows\System32\cmd.exe /A /Q /K C:\path\to\mdtenv.bat
- Run in: where you want to be after launching the shortcut


#### Install Mdt on Windows

##### Install in a predefined location

To install the library in a non system place,
i.e. defined above with CMAKE_INSTALL_PREFIX, the installation is:
```bash
mingw32-make install
```

### Cross compile Mdt for Windows on a Linux machine

My first attempt was to install Wine (then WineHQ)
and all tools and libraries on it, but this did not work for me.

Here we will use MXE to do cross-compilation.
You probably will use WineHQ to do some tests, in which case,
please remeber that WineHQ is possibly not able to run 64 bit executables.

#### Install WineHQ

WineHQ will be needed later to run the unit tests.
Wine can also be usefull to use, for example, dependency Walker,
or to check if your application works.

Please also note, for the nexte sections, that WineHQ probably not support 64 bit executable.

Using Ubuntu 16.04, I personally installed WineHQ successfully
by following the instructions avaiable on [WineHQ](https://www.winehq.org)
website, in the Download section.
Note: while running winecfg, I had to enable the option that emulates a desktop,
for some tools.
This is described here: [https://doc.ubuntu-fr.org/wine](https://doc.ubuntu-fr.org/wine)

#### Install other dependencies

Later, we will see that some dependecies must be copied to be able to run the unit tests.
For this, the objdump tool is needed.
On Debian, objdump is part of the binutils package:
```bash
sudo apt-get install binutils
```

#### Install MXE

MXE is a powerfull cross-compiler suite that will build all that we need.
Take a look at [MXE](http://mxe.cc) site for details.
As stated in the documentation, once MXE is built, it is not relocatable.
To reuse it for your own project, install it somwhere else than in Mdt source tree.
In my case, I choosed ~/opt/build/cross as base:
```bash
mkdir -p ~/opt/build/cross
```
cd build/release
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

#### Cross compile Mdt for Windows

Mdt uses CMake as build tool.
Compilation will be done in 2 phases:
 - Compile Mdt deploy utils for the native (Linux) platform
 - Cross-compile Mdt for Windows using MXE
For this, a helper script is avaliable in root of Mdt sources.

Go to Mdt source tree and create a directory to build a Windows 32 bit version:
```bash
mkdir -p build/cross/win32/release
cd build/cross/win32/release
```

Run the helper script by specify the MXE installation path,
target architecture and Mdt installation target path:
```bash
../../../../build_cross_linux_to_windows.sh --mxe-path=~/opt/build/cross/mxe/usr/bin -t=win32 --install-prefix=~/opt/mdt/win32/release ../../../../
```

To run all unit tests:
```bash
make test
```
CMake generated what is needed to call wine to run each unit test.

Note: for some reason, sometimes all tests fails, mostly at the first run.
In this case, simply run them again.

To run a single unit test, invoke wine, for example:
```bash
wine bin/mdtalgorithmtest.exe
```

To reompile some parts (or all):
```bash
make -j4
```

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

Write a CMakeLists.txt file:
```cmake
cmake_minimum_required(VERSION 3.3)

project(HelloWorld VERSION 0.0.1)

# Specify where to find Mdt
# Using a custom MDT_PREFIX_PATH has some advantages:
# - It can be reused, for example to specify CMAKE_MODULE_PATH
# - It solves the problem that CMAKE_PREFIX_PATH is ignored when cross-compiling with MXE
if(MDT_PREFIX_PATH)
  list(APPEND CMAKE_PREFIX_PATH "${MDT_PREFIX_PATH}")
  list(APPEND CMAKE_MODULE_PATH "${MDT_PREFIX_PATH}/share/cmake/modules")
endif()

# Specify where to find Qt5
if(QT_PREFIX_PATH)
  list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH}")
endif()

# On Windows, generate en env script
# The generated script will setup the PATH,
# like qtenv2.bat does.
include(MdtGenerateEnvScript)
mdt_generate_env_script(DESTINATION "${CMAKE_BINARY_DIR}")

# On Windows, RPATH do not exist
# To be able to run the application from the build tree,
# or run the unit tests, we have to put all binaries in the same directory
if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

# Thread support
find_package(Threads REQUIRED)

find_package(Qt5 COMPONENTS Widgets)
find_package(Mdt0 COMPONENTS ItemModel)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Rules to compile the application
# On Windows, we want a GUI executable, not a console
if(WIN32)
  add_executable(helloworld WIN32 HelloWorld.cpp)
else()
  add_executable(helloworld HelloWorld.cpp)
endif()
target_link_libraries(helloworld Mdt0::ItemModel Qt5::Widgets)

# Rules to install the application
install(
  TARGETS helloworld
  RUNTIME DESTINATION bin
)

# Rules to install application's dependencies
# Note that this is currently experimental
include(MdtDependenciesUtils)
find_package(mdtcpbindeps)
set_target_properties(helloworld PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
mdt_install_binary_dependencies(
  TARGET helloworld
  SEARCH_FIRST_PATH_PREFIX_LIST "${CMAKE_PREFIX_PATH}"
)

include(CPack)
```

## Build your project on Linux

Create a build directory and go to it:
```bash
mkdir -p build/release
cd build/release
```

If Mdt was installed in system standard way (for example using Debian packages),
and it uses system wide installed Qt5, following should be enouth:
```bash
cmake ../../
```

To use a non system wide installed Qt5 and Mdt, cmake must know it:
```bash
cmake -D MDT_PREFIX_PATH=~/opt/mdt/release -D QT_PREFIX_PATH=~/opt/qt/Qt5/5.9.1/gcc_64 ../../
```

It is also possible to specify a installation prefix:
```bash
cmake -D CMAKE_INSTALL_PREFIX=~/opt/helloworld/release ../../
```

For my personnal case, I also created a CMake cache file
to specify compiler flags.
To compile the application in debug mode, I use:
```bash
cmake -C ../../cmake/caches/DebugGcc.cmake -D MDT_PREFIX_PATH=~/opt/mdt/debug ../../
```

Build (-j4 is for parallel build, allowing max. 4 processes):
```bash
make -j4
```

You can also try to generate a archive:
```bash
cpack -G TBZ2 .
```

The generated archive should contain the application,
as well as its dependencies.
Note that Mdt provides some tools to fetch dependencies,
but this is currently experimental.

For more informations about how to deploy application,
a interesting start is [Qt for Linux - Deployment](http://doc.qt.io/qt-5/linux-deployment.html).


## Build your project on Windows

For this section, it will be considered that the HelloWorld source tree is: C:\Users\You\Documents\HelloWorld.

In the root of the source tree, create a directory called "build", and a subdirectory called "release".

Open a command prompt and cd to the created directory.
To initialize the build directory, the compiler tool set must be in the PATH.

Initialzise the build:
```bash
set PATH=C:\Path\To\MinGW\bin;%PATH%
cmake -G "MinGW Makefiles" -D MDT_PREFIX_PATH="C:/path/to/mdt" -D QT_PREFIX_PATH="C:/path/to/qt5 ..\..\
```

For my personnal case, I also created a CMake cache file
to specify compiler flags.
To compile the application in debug mode, I use:
```bash
set PATH=C:\Qt\Tools\mingw530_32\bin;%PATH%
cmake -C ..\..\cmake\caches\DebugGcc.cmake -G "MinGW Makefiles" -D MDT_PREFIX_PATH="C:/Users/Me/Documents/opt/Mdt/debug" -D QT_PREFIX_PATH="C:/Qt/5.9.1/mingw53_32" ..\..\
```

A script named mdtenv.bat should be generated in the built directory,
which setups a environment, like qtenv2.bat does.
It will set the PATH to include:
- Path to the executable directory of the compiler
- Path to the executable directory of the Qt5 library
- Path to the executable directory of Mdt

Execute mdtenv.bat:
```bash
mdtenv.bat
```

To run a command prompt with this environment set,
you can create a shortcut that runs cmd.exe and calls this mdtenv.bat.
Example of options of that shortcut could be:
- Target: C:\Windows\System32\cmd.exe /A /Q /K C:\path\to\mdtenv.bat
- Run in: where you want to be after launching the shortcut

Build:
```bash
mingw32-make -j4
```

You should be able to run the application
from the current command line prompt:
```bash
bin\helloworld.exe
```

To execute it outside current command prompt,
the application must be shipped with all its required dependencies
(mainly dlls)

Mdt has some experimental stuff to copy dependecies
and install them. You can try to generate a ZIP archive,
extract it, an run the application.

To generate a ZIP archive:
```bash
cpack -G ZIP .
```

For more informations about how to deploy application,
a interesting start is [Qt for Windows - Deployment](http://doc.qt.io/qt-5/windows-deployment.html).

## Using icons

Internally, Mdt uses [QIcon::fromTheme()](http://doc.qt.io/qt-5/qicon.html#fromTheme)
for elements that displays icons.
This method works out of the box on Linux-X11.

To make it also work on other platforms,
we have to provide a set of icons that are compatible with the
[freedesktop.org icon theme specifications](https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html) .

Mdt provides a copy for some compatible icon themes, like:
 - [Oxygen icons](https://github.com/KDE/oxygen-icons)


### Use icons for which Mdt provides a copy

At first, tell Qt where to find the icons.
For example, in main():
```cpp
#include <QApplication>
#include <QIcon>
#include <QStringList>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto themeSearchPaths = QIcon::themeSearchPaths();
  themeSearchPaths.append("share/icons");
  themeSearchPaths.removeDuplicates();
  QIcon::setThemeSearchPaths(themeSearchPaths);

  return app.exec();
}
```

Then, add a install() rule in the CMakeLists.txt:
```cmake
# Rules to install icons
if(MDT_PREFIX_PATH)
  install(
    DIRECTORY "${MDT_PREFIX_PATH}/share/icons/oxygen"
    DESTINATION "share/icons"
  )
endif()
```

Note that the icons are only available once the application is installed.



