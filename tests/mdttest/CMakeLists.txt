# CMake project file for Multi X Diag Tools
# * Unit test,  mdttest *
# * Directory: tests/mdttest *

# Projet
project (multidiagtools)

# Liste des fichiers sources
set (SOURCES_FILES 
          mdtTest.cpp
    )


# Binary lib
add_library (mdttest ${SOURCES_FILES})
target_link_libraries(mdttest Qt5::Widgets Qt5::Test)

# Minimal application
add_executable(minimalapp "minimalApp.cpp")
# target_link_libraries(minimalapp ${QT_LIBRARIES})
target_link_libraries(minimalapp Qt5::Widgets)

# Virtual method bench
add_executable(mdtvirtualmethodbench "mdtVirtualMethodBench.cpp")
target_link_libraries(mdtvirtualmethodbench "mdttest")
target_link_libraries(mdtvirtualmethodbench Qt5::Widgets)
