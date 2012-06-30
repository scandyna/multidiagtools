# CMake project file for Multi X Diag Tools
# * Unit test,  mdttest *
# * Directory: tests/mdttest *

# Projet
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtTest.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtTest.cpp
    )


# * mdtutils *

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Binary lib
add_library (mdttest ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libryries to link
target_link_libraries(mdttest ${QT_LIBRARIES})

# Minimal application
add_executable(minimalapp "minimalApp.cpp")
target_link_libraries(minimalapp ${QT_LIBRARIES})
