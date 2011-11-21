# CMake project file for Multi X Diag Tools
# * Part: mdtutils *
# * Directory: mdutils *

# Project
project (multixdiagtools)

# Libraries install directory
set (LIB_DEST_DIR "${PROJECT_SOURCE_DIR}/../../bin")

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # mdtError
          mdtErrorOut.h
          # Essais
          mdtParentChildTableModel.h
          mdtParentChildTableItem.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtAlgorithms.cpp
          mdtBuffer.cpp
          mdtTime.cpp
          # mdtError
          mdtError.cpp
          mdtErrorOut.cpp
          # Essais
          mdtParentChildTableModel.cpp
          mdtParentChildTableItem.cpp
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
target_link_libraries(mdtutils ${QT_LIBRARIES})

# Version statique
add_library (mdtutils_static STATIC ${SOURCES_FILES} )
target_link_libraries(mdtutils_static ${QT_LIBRARIES})
#set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")

# Destinations
install (TARGETS "mdtutils" DESTINATION "${LIB_DEST_DIR}")
install (TARGETS "mdtutils_static" DESTINATION "${LIB_DEST_DIR}")
