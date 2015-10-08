Multidiagtools
==============

Multidiagtools is a library that should help (in some future) interacting with some common instruments.
But, today it's just a set of C++ classes, and nothing is complete. Multidiagtool is based on Qt4, but some part of code is platform dependent.

Current state
=============

The library is currently just a set of C++ classes, and still in experimental state.
The branch master is just very old, and work progress only in experimental branch.

Getting multidiagtools
======================

Some binary packages are available as Debian archive, Windows installer or zip (for Windows) in the download section.
Theses packages are not up to date, so the best way to get library is with git.

From source with git

git clone git://github.com/scandyna/multidiagtools.git

Build

The source tree is handled by CMake. Some dependencies are needed to be able to build all.

Linux (Tested on Ubuntu 11.10)

Install required packages:

```bash
sudo apt-get install cmake make g++ libqt4-dev libqt4-network libqt4-sql libqt4-sql-mysql libqt4-sql-sqlite
```

```bash
cd to the source tree, and run:
./configure.sh && make clean && make
```
To run the unit test suite, you can use make test. All tests are available in tests sub-directory.

Windows

The library was tested on Windows XP and Wine. I use MinGW, so I will explain how to install and build all with it. The needed programs are:

*  CMake: http://www.cmake.org/
*  MinGW: http://www.mingw.org/
*  The simplest way is to use the packaged version from TDM-GCC project. I switched to TDM-GCC because I had trouble with MinGW packages. With mingw-get-inst-20120426's integrated catalog, gdb does not work, and online updated catalog provides GCC 4.7, witch cannot run Qt 4.8 binary distributed library. TDM installs gcc 4.6, and all worked fine for me on Windows XP 32bits.
*  Qt4: http://qt-project.org/downloads
*  Built version of Qt4 library are available. For example: Latest version Note: during install, a warning will be displayed, saying that winAPIversion is wrong. In my case, this was not a problem, all works fine (on native system).
*  NSIS (Optional, just to generate installer): http://nsis.sourceforge.net/Download

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

The best way to install is to generate packages, and install it. To create theses packages, run:

```
cpack .
```

Translations
============

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

