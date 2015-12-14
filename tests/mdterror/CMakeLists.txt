# CMake project file for Multi Diag Tools
# * Unit test for: mdterror *
# * Directory: tests/mdterror *

# Project
project (multidiagtools)

# NOTE: remove once migrated
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# find_package(Qt5Widgets REQUIRED) # Widgets finds its own dependencies (QtGui and QtCore).
# find_package(Qt5Network REQUIRED)
# find_package(Qt5Sql REQUIRED)
# find_package(Qt5Test REQUIRED)

# add_library (mdterror${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
# target_link_libraries(mdterror${MDTLIB_VERSION_SUFFIX} Qt5::Core)
# target_link_libraries(mdterror${MDTLIB_VERSION_SUFFIX} "qtsingleapplication${MDTLIB_VERSION_SUFFIX}")


# * mdtErrorTest executable *
add_executable(mdterrortest "mdtErrorTest.cpp")
target_link_libraries(mdterrortest "mdterror${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdterrortest "${CMAKE_BINARY_DIR}/src/mdterror/mdterror${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortest "mdttest")
target_link_libraries(mdterrortest "mdtutils${MDTLIB_VERSION_SUFFIX}")

# * mdtErrorGuiTest executable *
add_executable(mdterrorguitest "mdtErrorGuiTest.cpp")
target_link_libraries(mdterrorguitest "mdterror${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdterrortest "${CMAKE_BINARY_DIR}/src/mdterrorgui/mdterrorgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrorguitest "mdttest")
target_link_libraries(mdterrorguitest "mdtutils${MDTLIB_VERSION_SUFFIX}")