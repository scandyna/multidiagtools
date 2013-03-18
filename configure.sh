#!/bin/bash

# Script créé pour la partie C++ du projet
# Plateforme utilisée: Linux Ubuntu

# Fichier de cache CMake
CMAKE_CACHE_FILE="CMakeCache.txt"
# Répertoire de cache CMake
CMAKE_CACHE_DIR="CMakeFiles"

# Remove $1 if is a file
remove_file()
{
    if [ -f "$1" ]
    then
      echo "deleting $1 ..."
      rm "$1"
    fi
}

# Remove $1 if is a dir
remove_dir()
{
    if [ -d "$1" ]
    then
      echo "deleting $1 ..."
      rm -r "$1"
    fi
}

# Cache du projet
if [ -f "$CMAKE_CACHE_FILE" ] || [ -d "$CMAKE_CACHE_DIR" ]
then
  # On demande à l'utilisateur
  echo -n "Delete CMake cache ? [y/n] , (def: n): "
  read ANS
  if [ "$ANS" == "y" ]
  then
    # Fichiers de cache
    remove_file "$CMAKE_CACHE_FILE"
    remove_file "src/mdtdevice/$CMAKE_CACHE_FILE"
    remove_file "src/mdtport/$CMAKE_CACHE_FILE"
    remove_file "src/mdtserialport/$CMAKE_CACHE_FILE"
    remove_file "src/mdttools/$CMAKE_CACHE_FILE"
    remove_file "src/mdtutils/$CMAKE_CACHE_FILE"
    remove_file "src/mdtutilsgui/$CMAKE_CACHE_FILE"
    remove_file "src/qt-solutions/$CMAKE_CACHE_FILE"
    remove_file "src/qt-solutions/qtsingleapplication/$CMAKE_CACHE_FILE"
    remove_file "tools/uicnumber/$CMAKE_CACHE_FILE"
    remove_file "tools/mdtmodbusiotool/$CMAKE_CACHE_FILE"
    # Dossier de cache
    remove_dir "$CMAKE_CACHE_DIR"
    remove_dir "src/mdtdevice/$CMAKE_CACHE_DIR"
    remove_dir "src/mdtport/$CMAKE_CACHE_DIR"
    remove_dir "src/mdtserialport/$CMAKE_CACHE_DIR"
    remove_dir "src/mdttools/$CMAKE_CACHE_DIR"
    remove_dir "src/mdtutils/$CMAKE_CACHE_DIR"
    remove_dir "src/mdtutilsgui/$CMAKE_CACHE_DIR"
    remove_dir "src/qt-solutions/$CMAKE_CACHE_DIR"
    remove_dir "src/qt-solutions/qtsingleapplication/$CMAKE_CACHE_DIR"
    remove_dir "tests/mdtdevice/$CMAKE_CACHE_DIR"
    remove_dir "tests/mdtserialport/$CMAKE_CACHE_DIR"
    remove_dir "tests/mdttest/$CMAKE_CACHE_DIR"
    remove_dir "tests/mdtutils/$CMAKE_CACHE_DIR"
    remove_dir "tests/mdtutilsgui/$CMAKE_CACHE_DIR"
    remove_dir "tools/uicnumber/$CMAKE_CACHE_DIR"
    remove_dir "tools/mdtmodbusiotool/$CMAKE_CACHE_DIR"
  fi
fi

# CMake
cmake . -Wdev

if [ $? != "0" ]
then
  echo "Error during cmake ."
  exit 1
fi

echo "** cmake . done , you schould be able to compile with make"
