# Fichier de projet pour Multi X Diag Tools

# CMake ressources:
#  http://www.cmake.org/cmake/help/cmake_tutorial.html
#  http://www.cmake.org/Wiki/CMake_Useful_Variables
#  http://qt-quarterly.developpez.com/qq-34/cmake/
#  http://matthieu-brucher.developpez.com/tutoriels/cpp/qt/compilation/

# CMake project file for Multi X Diag Tools
# * root project file *

# Version requise de CMake
cmake_minimum_required ( VERSION 2.6 )

# Project
project (multixdiagtools)

# Utilisation des assertions , positionner à 0 pour désactiver
set (USE_ASSERT 1)
#Debug ou Release
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Version du projet
set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (VERSION_MICRO 1)

# Fichier de configuration
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
)

# Quelques options passées directement au compilateur
ADD_DEFINITIONS( "-Wall -ansi -lm" )

# Support de Qt4
SET(QT_USE_QTTEST TRUE)
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Répertoires de headers
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/src/mdtutils")

# Exécutable pour tests
add_executable (essais src/main.cpp)
# Librairies à lier
target_link_libraries(essais "mdtutils")
#target_link_libraries(unMain ${QT_LIBRARIES})

#install (TARGETS "unMain" DESTINATION "${PROJECT_SOURCE_DIR}/bin")

# *** Partie librairies ***

# * mdtutils *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtutils")


# *** Partie Tests unitaires ***

# * Compilation des tests unitaires *
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")

# ** Exécution des tests unitaires **
enable_testing()
add_test (mdtUtilsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtUtilsTest)
