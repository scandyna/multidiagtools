# Install Mdt Dependencies

Mdt uses [CMake](https://cmake.org) as build management system.
It also depend on a C++14 compliant compiler
(f.ex. allmost recent version of [GCC](https://gcc.gnu.org) or [Clang](https://clang.llvm.org) ).
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
sudo apt-get install binutils patchelf
```

### Install Qt5 on Linux

Mdt needs a recent version of Qt5.

#### Install Qt5 provided by the distribution

If your distribution provides the required version, you can use that.
For example, on Ubuntu:
```bash
sudo apt-get install qtbase5-dev qtbase5-dev-tools libqt5gui5 libqt5network5 libqt5sql5 libqt5sql5-mysql libqt5sql5-psql libqt5sql5-sqlite libqt5test5 libqt5widgets5
```

Optionnally, documentation can also be installed:
```bash
sudo apt-get install qtbase5-doc qtbase5-doc-html qtbase5-examples
```

#### Install Qt5 if not provided by the distribution

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

## Windows

### Dependecies Walker

To find some informations about dll's, such as missing dependencies,
[Dependency Walker](http://www.dependencywalker.com/) can be helpful.

Note: on Linux, using Wine, probably only the 32 bit version will work
(choose x86, not x64).

### CMake

Mdt uses [CMake](https://cmake.org/) as build tool.
Choose a installer that matches your platform from the [Download](https://cmake.org/download/) section.
Here are some options that I choosed (all other I keeped default):
* "Add CMake to the system PATH for the current user"

### Install Qt5 and Gcc

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


# Install Mdt

This section will explain how to install Mdt.

At first, [install Mdt dependencies](#install-mdt-dependencies) .

To see how to get the source code and compile Mdt,
see the [README](README.md) file.

## Linux

### Binary release

Currently, Bzip2 archives are available
in [releases](https://github.com/scandyna/multidiagtools/releases).

Download the one that matches your platform
and store it somewhere, for example ~/opt/mdt .

Cd to the location where the archive and extract it:
```bash
cd ~/opt/mdt
tar -xjf Mdt0-0.3.6-Qt-5.9-Linux-GNU-x86_64.tar.bz2
```

Cd to the extracted directory:
```bash
cd Mdt0-0.3.6-Qt-5.9-Linux-GNU-x86_64
```

Notice that the name of the archive
tells with which version of Qt it was compiled.

If you don't have a recent enough version of Qt
installed on your system (in /usr/lib),
run the [post install script](#post-install-script-on-linux) .

To check the installation, run:
```bash
bin/mdtcpbindeps -h
```

### Post install script on Linux

Mdt is based (and also depends) on Qt5 library.

Some part of the user's platform environment is not known while generating the package
(Typically the path to a recent enough Qt library).
For this reason, a post-install script must be executed.

From the root of the installed Mdt, run:
```bash
cmake -D QT_PREFIX_PATH=/path/to/Qt -P MdtPostInstall.cmake
```

A file called QtPrefixPath.txt is also generated.
It tells where the used Qt library is installed.

The RPATH of Mdt libraries and tools is also set.

To check the installation, run:
```bash
bin/mdtcpbindeps -h
```

## Windows

### Binary release

Currently, zip archives are available
in [releases](https://github.com/scandyna/multidiagtools/releases).

Download the one that matches your platform and store it somewhere,
for example in %HOMEPATH%\Documents\opt\Mdt .

Extract the archive.

Note: when using Explorer to extract a Zip archive,
it will create a folder having the same name as the zip file.
Because the zip file contains a folder with the same name,
the result will be archiveName\archiveName\content.
To avoid this, remove the archive name from the suggested path.

Notice that the name of the archive
tells with which version of Qt it was compiled.

To make Mdt work,
run the [post install script](#post-install-script-on-windows) .

### Post install script on Windows

Mdt is based (and also depends) on Qt5 library.

Some part of the user's platform environment is not known while generating the package
(Typically the path to a recent enough Qt library).
For this reason, a post-install script must be executed.

Open a command line and cd to the Mdt intallation:
```bash
cd %HOMEPATH%\Documents\opt\Mdt\Mdt0-0.3.6-Qt5.9-Windows-GNU-i386
```

From the root of the installed Mdt, run:
```bash
cmake -D QT_PREFIX_PATH=\path\to\Qt -P MdtPostInstall.cmake
```

A file called QtPrefixPath.txt is also generated.
It tells where the used Qt library is installed.

A mdtenv.bat script is also genrated.
Executing it will set the PATH to include:
- Path to the executable directory of the compiler
- Path to the executable directory of the Qt5 library
- Path to the executable directory of the Mdt library

To check the installation, run:
```bash
mdtenv.bat
bin\mdtcpbindeps.exe -h
```

To run a command prompt with this environment set,
you can create a shortcut that runs cmd.exe and calls mdtenv.bat.
Example of options of that shortcut could be:
- Target: C:\Windows\System32\cmd.exe /A /Q /K C:\path\to\mdtenv.bat
- Run in: where you want to be after launching the shortcut
