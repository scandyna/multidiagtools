# Install Mdt

This section will explain how to install Mdt.

To see how to get the source code and compile Mdt,
see the [README](README.md) file.

Mdt is based on [Qt5](https://www.qt.io/) ,
and also depends on it.

## Linux

Before using Mdt, you have to install a recent enough version of Qt5 .

Take a look in the  [README](README.md/#install-qt5-on-linux)
file for some help.

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

Before using Mdt, you have to install a recent enough version of Qt5 .

Take a look in the  [README](README.md/#install-qt5-and-gcc)
file for some help.

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
Executing it will setup the PATH to find Qt5 and Mdt binaries.

To check the installation, run:
```bash
mdtenv.bat
bin\mdtcpbindeps.exe -h
```
