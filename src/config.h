// Options de configuration, selon: http://www.cmake.org/cmake/help/cmake_tutorial.html

/*
  Le fichier config.h.in contient des constantes
  dont les valeurs, entre '@' , seront remplacé par cmake
  Le fichier de sortie, config.h, sera généré par cmake
  Voir le fichier CMakeLists.txt pour les valeurs des définitions
*/

/* Version */
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_MICRO 1

/* Utilisation des assertions avec assert() */
#define USE_ASSERT 1

/*
  Constantes fixes
*/
