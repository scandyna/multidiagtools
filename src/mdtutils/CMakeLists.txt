# Fichier de projet pour Multi X Diag Tools
# partie librairie d'utilitaires

# Ressources CMake:
#  http://www.cmake.org/cmake/help/cmake_tutorial.html
#  http://www.cmake.org/Wiki/CMake_Useful_Variables

# Projet
project (mdtutils)

# Répertoire de destination
set (LIB_DEST_DIR "${PROJECT_SOURCE_DIR}/../../bin")

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          essais.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          essais.cpp
    )

# Liste des fichiers ui
set (FORM_FILES

    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

    )

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtutils SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})

# Version statique
add_library ("mdtutils_static" STATIC ${SOURCES_FILES} )
#set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")

# Destinations
install (TARGETS "mdtutils" DESTINATION "${LIB_DEST_DIR}")
install (TARGETS "mdtutils_static" DESTINATION "${LIB_DEST_DIR}")
