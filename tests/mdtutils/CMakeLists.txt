# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdutils *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtAlgorithmsTest.h
          mdtBufferTest.h
          mdtFrameTest.h
          mdtErrorTest.h
          mdtFrameCodecTest.h
          mdtFileTest.h
          mdtUicNumberTest.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtAlgorithmsTest.cpp
          mdtBufferTest.cpp
          mdtFrameTest.cpp
          mdtErrorTest.cpp
          mdtFrameCodecTest.cpp
          mdtFileTest.cpp
          mdtUicNumberTest.cpp
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
add_executable (mdtUtilsTest ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraires à lier
target_link_libraries(mdtUtilsTest "mdtutils")
target_link_libraries(mdtUtilsTest "mdttest")
target_link_libraries(mdtUtilsTest ${QT_LIBRARIES})
