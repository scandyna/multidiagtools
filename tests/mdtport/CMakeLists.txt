# CMake project file for Multi X Diag Tools
# * Unit test for: mdtport *
# * Directory: tests/mdtport *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtPortTest.h
          # Utils
          mdtTcpServer.h
          mdtTcpServerThread.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtPortTest.cpp
          # Utils
          mdtTcpServer.cpp
          mdtTcpServerThread.cpp
          # Main
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
add_executable (mdtPortTest ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraires à lier
target_link_libraries(mdtPortTest "mdtutils")
target_link_libraries(mdtPortTest "mdtport")
target_link_libraries(mdtPortTest "mdttest")
target_link_libraries(mdtPortTest ${QT_LIBRARIES})
