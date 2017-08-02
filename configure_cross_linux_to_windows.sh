#!/bin/bash

print_help()
{
  echo ""
  echo "Helper script to cross compile Mdt on a Linux host for Windows"
  echo ""
  echo "Usage: $0 --mxe-path=/path/to/mxe --install-prefix=/where/to/install/mdt --target=target"
  echo ""
  echo "Options:"
  echo " --mxe-path: Path to the MXE installation. Must be set to MXE root/usr/bin (see MXE documentation for more details)"
  echo " --install-prefix: Path to where Mdt must be installed"
  echo " -t or --target: target architecture. Can be win32 (for a X86 32 bit architecture) or win64 (for a X86 64 bit architecture)"
  echo " -h or --help: print this help"
}

# Default parameters
MXE_PATH=""
INSTALL_PREFIX="/usr/local"
CMAKE_COMMAND=""

# Set CMake command depending on given target
# Arguments:
#  $1 : target. Can be win32 or win64
set_cmake_command()
{
  target=$1
  case $target in
    win32)
      CMAKE_COMMAND="i686-w64-mingw32.shared.posix-cmake"
      ;;
    win64)
      CMAKE_COMMAND="x86_64-w64-mingw32.shared.posix-cmake"
      ;;
    *)
      echo "Target $target is not supported"
      exit 1
  esac
}

# Set MXE_PATH
# Arguments:
#  $1 : path as given by the user
set_mxe_path()
{
  # We have to replace ~/ by the full path,
  # else we have some surprises
  # (f.ex. test -d allways fails)
  MXE_PATH=${1/\~\//"$HOME/"}

  if [ ! -d "$MXE_PATH" ]
  then
    echo "MXE path $MXE_PATH do not refer to a existing directory"
    exit 1
  fi
}

# Set INSTALL_PREFIX
# Arguments:
#  $1 : path as given by the user
set_install_prefix()
{
  INSTALL_PREFIX=${1/\~\//"$HOME/"}
}

#
# Main
#

# Parse arguments
while [ "$1" != "" ]
do
  key=`echo $1 | awk -F= '{print $1}'`
  value=`echo $1 | awk -F= '{print $2}'`
  case $key in
    -h|--help)
      print_help
      exit
      ;;
    --mxe-path)
      set_mxe_path "$value"
      ;;
    --install-prefix)
      set_install_prefix "$value"
      ;;
    -t|--target)
      set_cmake_command $value
      ;;
    *)
      echo "Unknown argument: $key"
      print_help
      exit 1
      ;;
  esac
  shift
done

# Check that we have all parameters
if [ "$MXE_PATH" == "" ]
then
  echo "MXE path is missing"
  exit 1
fi
if [ "$INSTALL_PREFIX" == "" ]
then
  echo "Install prefix is missing"
  exit 1
fi
if [ "$CMAKE_COMMAND" == "" ]
then
  echo "Target is missing"
  exit 1
fi

echo "Using following setup:"
echo " MXE_PATH: $MXE_PATH"
echo " INSTALL_PREFIX: $INSTALL_PREFIX"
echo " CMAKE_COMMAND $CMAKE_COMMAND"
