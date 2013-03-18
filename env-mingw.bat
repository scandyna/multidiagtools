@echo off

REM ===============================================================================
REM    Build PATH that is needed for developpment
REM ===============================================================================

REM ===============================================================================
REM    Try to find MinGW
REM ===============================================================================
set MINGW_DIR=
if exist "C:\MinGW32\" (
  set MINGW_DIR=C:\MinGW32
)
if exist "C:\MinGW\" (
  set MINGW_DIR=C:\MinGW
)
set MINGW_BIN=%MINGW_DIR%\bin

REM MinGW install dir
REM set MINGW_DIR=C:\MinGW32
REM set MINGW_DIR=C:\MinGW
REM GNU MinGW developpment environnement
REM Create the PATH var

REM ===============================================================================
REM    Try to find CMake
REM ===============================================================================
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
REM    Try to find CMake
REM ===============================================================================
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
set QMAKESPEC=win32-g++

REM Qt4 config
REM set QTDIR=C:\Qt\2010.05\qt


REM Multidiagtools libraries (needed for tests)
set MDTLIBS=src\mdtutils;src\mdtutilsgui;src\qt-solutions\qtsingleapplication

REM ===============================================================================
REM    Build PATH
REM ===============================================================================

echo Adding %CMAKE_BIN% to PATH
set PATH=%CMAKE_BIN%

echo Adding %QTDIR%\bin to PATH
set PATH=%PATH%;%QTDIR%\bin

echo Adding %MINGW_BIN% to PATH
set PATH=%PATH%;%MINGW_BIN%

echo Adding %SystemRoot%\System32 to PATH
set PATH=%PATH%;%SystemRoot%\System32

echo Adding %SystemRoot% to PATH
set PATH=%PATH%;%SystemRoot%

echo Adding %MDTLIBS% to PATH
set PATH=%PATH%;%MDTLIBS%
