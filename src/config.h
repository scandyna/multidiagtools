// Options de configuration, selon: http://www.cmake.org/cmake/help/cmake_tutorial.html

/*
  Le fichier config.h.in contient des constantes
  dont les valeurs, entre '@' , seront remplacé par cmake
  Le fichier de sortie, config.h, sera généré par cmake
  Voir le fichier CMakeLists.txt pour les valeurs des définitions
*/

/* Version */
#define MDTLIB_VERSION_MAJOR 0
#define MDTLIB_VERSION_MINOR 2
#define MDTLIB_VERSION_MICRO 5

/* Utilisation des assertions avec assert() */
#define USE_ASSERT 1

/*
  Constantes fixes
*/
