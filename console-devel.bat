@echo off
REM Environnement de développement GNU
REM Cré une variable PATH

REM Dossier d'installation MinGW
REM set MINGW_DIR=C:\Qt\2010.05\mingw
set MINGW_DIR=C:\MinGW

REM Dossier d'installation CMake
set CMAKE_DIR=C:\Program Files\CMake 2.8

REM Configuration de Qt4
set QTDIR=C:\Qt\2010.05\qt
set QMAKESPEC=win32-g++


REM --- Construction des variables ----

REM MinGW
set MINGW_BIN=%MINGW_DIR%\bin

REM CMake
set CMAKE_BIN=%CMAKE_DIR%\bin

REM ---- Construction du PATH ----

echo Adding %CMAKE_BIN% to PATH
set PATH=%CMAKE_BIN%

echo Adding C:\Qt\2010.05\qt\bin to PATH
set PATH=%PATH%;C:\Qt\2010.05\qt\bin

echo Adding C:\Qt\2010.05\bin to PATH
set PATH=%PATH%;C:\Qt\2010.05\bin

echo Adding %MINGW_BIN% to PATH
set PATH=%PATH%;%MINGW_BIN%

echo Adding %SystemRoot%\System32 to PATH
set PATH=%PATH%;%SystemRoot%\System32
