# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdtdevice *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtDeviceTest.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtDeviceTest.cpp
          main.cpp
    )


# * mdtutils *

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Exécutable
add_executable (mdtDeviceTest ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraires à lier
target_link_libraries(mdtDeviceTest "mdtutils")
target_link_libraries(mdtDeviceTest "mdtdevice")
target_link_libraries(mdtDeviceTest "mdttest")
target_link_libraries(mdtDeviceTest ${QT_LIBRARIES})