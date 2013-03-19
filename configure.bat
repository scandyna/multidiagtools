@echo off

REM ===============================================================================
REM    Helper script for CMake on Win32
REM ===============================================================================

REM Crate minimal PATH for developpement environnement
call env-mingw.bat

REM CMake cache file
set CMAKE_CACHE_FILE="CMakeCache.txt"


REM Project cache
if exist %CMAKE_CACHE_FILE% (
  echo Deleting %CMAKE_CACHE_FILE%
  del %CMAKE_CACHE_FILE%
)

REM Run CMake with MinGW generator
cmake -G "MinGW Makefiles" . -Wdev
if ERRORLEVEL 0 (
  echo ** cmake . done , you schould be able to compile with make or mingw32-make
)
