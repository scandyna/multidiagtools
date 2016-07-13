#!/bin/bash

# Simple script that inits build tree



#
# Helper functions
#

# Create directory passed as first argument if not exists
create_directory()
{
  # Check if a file with requested name allready exists
  if [ -f "$1" ]
  then
    echo "cannot create directory $1. A file with this name allready exists. Aborting"
    exit 1
  fi
  # Create directory if not exists
  if [ ! -d "$1" ]
  then
    echo "creating directory $1"
    mkdir "$1"
  fi
  # On error, we abort here
  if [ $? != "0" ]
  then
    exit $?
  fi
}

# Check if directory is clean
# Arguments:
#  $1 : relative path to directory
check_directory_clean()
{
  if [ $(ls "$1" | wc -c) != "0" ]
  then
    echo "directory $1 is not empty."
    return 1  # 1 = false in bash
  fi

  return 0 # 0 = true
}

# Init directory
# Arguments:
#  $1 : relative path to directory
#  $2 : build type (Release, Debug, whatever CMake supports)
init_directory()
{
  initial_path="$PWD"

  if [ ! -d "$1" ]
  then
    echo "directory $1 not exists"
    exit 1
  fi
  cd "$1"
  # We not want to delete things,
  # so if a project allready exists, we abort
  if ! check_directory_clean "$1"
  then
    cd "$initial_path"
    echo "aborting $2 build"
    return
  fi
  # Init..
  cmake ../../ -DCMAKE_BUILD_TYPE="$2"
  cd "$initial_path"
}

#
# Main
#
build_path=$(realpath "build")

create_directory "$build_path"
# We not want to delete things,
# so if a project allready exists, we abort
if ! check_directory_clean "$build_path"
then
  echo "if build is a build directory, please clean it an try again"
  exit 1
fi
create_directory "$build_path/debug"
create_directory "$build_path/release"
init_directory "$build_path/debug" "Debug"
init_directory "$build_path/release" "Release"
