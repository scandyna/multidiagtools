@echo off

REM ===============================================================================
REM    Build PATH that is needed for developpment
REM ===============================================================================

REM ===============================================================================
REM    Try to find MinGW
REM ===============================================================================
echo Searching MinGW...
set MINGW_DIR=
if exist "C:\MinGW32\" (
  set MINGW_DIR=C:\MinGW32
)
if exist "C:\MinGW\" (
  set MINGW_DIR=C:\MinGW
)
REM set MINGW_BIN=%MINGW_DIR%\bin

REM MinGW install dir
REM set MINGW_DIR=C:\MinGW32
REM set MINGW_DIR=C:\MinGW
REM GNU MinGW developpment environnement
REM Create the PATH var

REM ===============================================================================
REM    Try to find CMake
REM ===============================================================================
echo Searching CMake...
set CMAKE_DIR=
if exist "C:\Program Files\CMake 2.8\" (
  set CMAKE_DIR=C:\Program Files\CMake 2.8
)
if exist "C:\Program Files\CMake 2.9\" (
  set CMAKE_DIR=C:\Program Files\CMake 2.9
)
set CMAKE_BIN=%CMAKE_DIR%\bin

REM CMake install dir
REM set CMAKE_DIR=C:\Program Files\CMake 2.8

REM ===============================================================================
REM    Try to find Qt4
REM ===============================================================================
echo Searching Qt4...
set QTDIR=
if exist "C:\Qt\4.8.0\" (
  set QTDIR=C:\Qt\4.8.0
)
if exist "C:\Qt\4.8.1\" (
  set QTDIR=C:\Qt\4.8.1
)
if exist "C:\Qt\4.8.2\" (
  set QTDIR=C:\Qt\4.8.2
)
if exist "C:\Qt\4.8.3\" (
  set QTDIR=C:\Qt\4.8.3
)
if exist "C:\Qt\4.8.4\" (
  set QTDIR=C:\Qt\4.8.4
)
REM ===============================================================================
REM    Try to find Qt5
REM    If found, we use MingGW priveided
REM ===============================================================================
echo Searching Qt5...

if exist "C:\Qt\5.0.2\" (
  set QTDIR=C:\Qt\5.0.2\5.0.2\mingw47_32
)
if exist "C:\Qt\5.0.2\Tools\MinGW" (
  set MINGW_DIR=C:\Qt\5.0.2\Tools\MinGW
)

if exist "C:\Qt\Qt5.0.2\" (
  set QTDIR=C:\Qt\Qt5.0.2\5.0.2\mingw47_32
)
if exist "C:\Qt\Qt5.0.2\Tools\MinGW" (
  set MINGW_DIR=C:\Qt\Qt5.0.2\Tools\MinGW
)

set QMAKESPEC=win32-g++


REM ===============================================================================
REM    Try to find Qwt
REM ===============================================================================
echo Searching Qwt...
set QWTDIR=
if exist "C:\Qwt-6.0.0\" (
  set QWTDIR=C:\Qwt-6.0.0
)
if exist "C:\Qwt-6.0.1\" (
  set QWTDIR=C:\Qwt-6.0.1
)
if exist "C:\Qwt-6.0.2\" (
  set QWTDIR=C:\Qwt-6.0.2
)


REM ===============================================================================
REM    Multidiagtools libraries (needed for tests)
REM ===============================================================================
set MDTLIBS=src\mdtdevice;src\mdtport;src\mdtserialport;src\mdttools;src\mdtutils;src\mdtutilsgui;src\qt-solutions\qtsingleapplication

REM ===============================================================================
REM    Build PATH
REM ===============================================================================

set MINGW_BIN=%MINGW_DIR%\bin
echo Adding %MINGW_BIN% to PATH
set PATH=%MINGW_BIN%

echo Adding %CMAKE_BIN% to PATH
set PATH=%PATH%;%CMAKE_BIN%

echo Adding %QTDIR%\bin to PATH
set PATH=%PATH%;%QTDIR%\bin

echo Adding %QWTDIR%\lib to PATH
set PATH=%PATH%;%QWTDIR%\lib

echo Adding %SystemRoot%\System32 to PATH
set PATH=%PATH%;%SystemRoot%\System32

echo Adding %SystemRoot% to PATH
set PATH=%PATH%;%SystemRoot%

echo Adding %MDTLIBS% to PATH
set PATH=%PATH%;%MDTLIBS%
