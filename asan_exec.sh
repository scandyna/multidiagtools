#!/bin/bash

# Helper sript to enable/disable ASAN memory leaks detection

# ASAN suppression file
root_directory=$(dirname "$0")
suppression_file="$root_directory/asan_suppressions.txt"

# Global on/off variable
# 0 means disabled
# 1 means enabled
leaks_detection="1"

print_help()
{
  echo "For interactive usage: asan_exec.sh executable"
  echo "Or, directly tell to enable or disable leak detections: asan_exec.sh --detect_leaks=[1|0] executable"
}

# Ask the user if he wanst to enable or disable leaks detection
ask_on_off()
{
  echo -n "Enable ASAN leaks detection ? [y|yes|n|no(default:no)]: "
  read answer
  if [ "$answer" == "y" ]
  then
    leaks_detection="1"
  elif [ "$answer" == "yes" ]
  then
    leaks_detection="1"
  else
    leaks_detection="0"
  fi
}

parse_detect_leaks_option()
{
  if [ "$1" == "--detect_leaks=1" ]
  then
    leaks_detection="1"
  elif [ "$1" == "--detect_leaks=0" ]
  then
    leaks_detection="0"
  else
    echo "Error: unknown option $1"
    print_help
    exit 1
  fi
}

setup_variables()
{
  export ASAN_OPTIONS="detect_leaks=$leaks_detection"
  export LSAN_OPTIONS="suppressions=$suppression_file"
}

# Main

if (( $# == 0 ))
then
  echo "Error: expected a command to execute - Abort"
  print_help
  exit 1
fi

if (( $# == 1 ))
then
  ask_on_off
  setup_variables
  exec "${@:1}"
else
  parse_detect_leaks_option "$1"
  setup_variables
  exec "${@:2}"
fi




