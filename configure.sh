#!/bin/bash

# Script créé pour la partie C++ du projet
# Plateforme utilisée: Linux Ubuntu

# Fichier de cache CMake
CMAKE_CACHE_FILE="CMakeCache.txt"
# Répertoire de cache CMake
CMAKE_CACHE_DIR="CMakeFiles"

# Cache du projet
if [ -f "$CMAKE_CACHE_FILE" ] || [ -d "$CMAKE_CACHE_DIR" ]
then
  # On demande à l'utilisateur
  echo -n "Delete CMake cache ? [y/n] , (def: n): "
  read ANS
  if [ "$ANS" == "y" ]
  then
    # Fichier de cache
    if [ -f "$CMAKE_CACHE_FILE" ]
    then
      rm "$CMAKE_CACHE_FILE"
    fi
    # Dossier de cache
    if [ -d "$CMAKE_CACHE_DIR" ]
    then
      rm -r "$CMAKE_CACHE_DIR"
    fi
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
