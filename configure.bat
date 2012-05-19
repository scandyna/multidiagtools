@echo off

REM Crate PATH for developpement environnement
call env-mingw.bat

REM Helper script for CMake on Win32

REM CMake cache file
set CMAKE_CACHE_FILE="CMakeCache.txt"


REM Project cache
REM NOTE: demander la question à l'utilisateur reste à implémenter
if exist %CMAKE_CACHE_FILE% (
  echo Deleting %CMAKE_CACHE_FILE%
  del %CMAKE_CACHE_FILE%
)

REM Run CMake with MinGW generator
cmake -G "MinGW Makefiles" . -Wdev
if "%ErrorLevel%" NEQ "0" (
  goto end
)

echo ** cmake . done , you schould be able to compile with mingw32-make

REM Fin
:end
echo Fin
