@echo off

REM GNU MinGW developpment environnement
REM Create the PATH var

REM MinGW install dir
set MINGW_DIR=C:\MinGW32

REM CMake install dir
set CMAKE_DIR=C:\Program Files\CMake 2.8

REM Qt4 config
REM set QTDIR=C:\Qt\2010.05\qt
set QTDIR=C:\Qt\4.8.2
set QMAKESPEC=win32-g++

REM Multidiagtools libraries (needed for tests)
set MDTLIBS=src\mdtutils;src\mdtutilsgui;src\qt-solutions\qtsingleapplication

REM --- Build vars ----

REM MinGW
set MINGW_BIN=%MINGW_DIR%\bin

REM CMake
set CMAKE_BIN=%CMAKE_DIR%\bin

REM ---- Build PATH ----

echo Adding %CMAKE_BIN% to PATH
set PATH=%CMAKE_BIN%

REM echo Adding C:\Qt\2010.05\qt\bin to PATH
REM set PATH=%PATH%;C:\Qt\2010.05\qt\bin

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
