# Fichier de projet pour Multi X Diag Tools
# Tests unitaires

# Ressources CMake:
#  http://www.cmake.org/cmake/help/cmake_tutorial.html
#  http://www.cmake.org/Wiki/CMake_Useful_Variables

# Projet
project (multixdiagtools)

# Accès aux fichiers d'include du projet
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Fichiers sources
set (SOURCES
          mdtutils/essaisTest.cpp
    )

# * mdtutils *
add_executable (essaisTest "${SOURCES}")
# Libraires à lier
target_link_libraries(essaisTest "mdtutils")
target_link_libraries(essaisTest ${QT_LIBRARIES})