# Project file for Multi Diag Tools

# CMake ressources:
#  http://www.cmake.org/cmake/help/cmake_tutorial.html
#  http://www.cmake.org/Wiki/CMake_Useful_Variables
#  http://qt-quarterly.developpez.com/qq-34/cmake/
#  http://matthieu-brucher.developpez.com/tutoriels/cpp/qt/compilation/
#  http://www.cmake.org/Wiki/CMake:CPackConfiguration
# CPack ressources:
#  http://www.vtk.org/Wiki/CMake:Component_Install_With_CPack

# CMake project file for Multi Diag Tools
# * root project file *

# Version requise de CMake
# CMake minimal requiered version
cmake_minimum_required(VERSION 2.6)

# Project
project(multidiagtools)

# Utilisation des assertions , positionner à 0 pour désactiver (obselète avec Q_ASSERT() ?)
set(USE_ASSERT 1)

#Debug or Release
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Project version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_MICRO 3)

# Config file
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
)

# Some flags for compiler
add_definitions("-Wall -ansi -lm")


# Enable Qt4 support
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# Headers directories
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtutils")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtutilsgui")
include_directories("${PROJECT_SOURCE_DIR}/src/qt-solutions/qtsingleapplication")
# NOTE: Support Windows pas terminé
if(UNIX)
include_directories("${PROJECT_SOURCE_DIR}/src/mdtdevice")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtport")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtserialport")
include_directories("${PROJECT_SOURCE_DIR}/src/mdttools")
endif(UNIX)

# *** Librairies section ***

# Build the static variant of libraries
set(MDT_BUILD_STATIC_LIBS 0)

# Libraries and headers install directories
if(WIN32)
  set(LIB_DEST_DIR "bin")
  set(HEADERS_DEST_DIR "include/mdt")
else(WIN32)
  set(LIB_DEST_DIR "lib")
  set(HEADERS_DEST_DIR "include/mdt")
endif(WIN32)

# * QtSolutions *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/qt-solutions")

# * mdtutils *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtutils")

# * mdtutilsgui *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtutilsgui")

# NOTE: Support Windows pas terminé
if(UNIX)
# * mdtdevice *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtdevice")

# * mdtport *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtport")

# * mdtserialport *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtserialport")

# * mdttools *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdttools")
endif(UNIX)

# *** Tools section (executables) ***
add_subdirectory("${PROJECT_SOURCE_DIR}/tools")

# *** Mini test programm section ***

# NOTE: Support Windows pas terminé
if(UNIX)

# Exécutable pour tests
add_executable (essais src/main.cpp)
# Librairies à lier
target_link_libraries(essais "mdtutils")
target_link_libraries(essais "mdtutilsgui")
target_link_libraries(essais "mdtport")
target_link_libraries(essais "mdtserialport")
target_link_libraries(essais "mdtdevice")
target_link_libraries(essais "mdttools")
target_link_libraries(essais ${QT_LIBRARIES})

endif(UNIX)

# *** Unit tests section ***

# * Unit tests compilation *
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")

# * Unit tests execution *
enable_testing()
add_test (mdtUtilsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtUtilsTest)
# NOTE: Support Windows pas terminé
if(UNIX)
add_test (mdtPortTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtPortTest)
add_test (mdtSerialPortTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtserialport/mdtSerialPortTest)
add_test (mdtDeviceTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdevice/mdtDeviceTest)
endif(UNIX)

# *** Install section ***

# Some Linux specific files
if(UNIX)
  install(FILES "${PROJECT_SOURCE_DIR}/dist/debian/etc/udev/rules.d/40-usbtmc.rules" DESTINATION "/etc/udev/rules.d/" COMPONENT lib)
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/data" DESTINATION "/usr/share/mdt/" COMPONENT lib)
endif(UNIX)

# Some Windows specific files
if(WIN32)
  find_library(LIBGCCDLL NAMES libgcc_s_dw2-1.dll)
  find_library(LIBSTDCPP6DLL NAMES libstdc++-6.dll)
  find_library(MINGWM10DLL NAMES mingwm10.dll)
  # Qt4 dll's - choose correct dll regarding on Debug or Release build type
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    find_library(QTCOREDLL NAMES QtCored4.dll)
    find_library(QTGUIDLL NAMES QtGuid4.dll)
    find_library(QTNETWORKDLL NAMES QtNetworkd4.dll)
  else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    find_library(QTCOREDLL NAMES QtCore4.dll)
    find_library(QTGUIDLL NAMES QtGui4.dll)
    find_library(QTNETWORKDLL NAMES QtNetwork4.dll)
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  install(FILES ${LIBGCCDLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(FILES ${LIBSTDCPP6DLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(FILES ${MINGWM10DLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(FILES ${QTCOREDLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(FILES ${QTGUIDLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(FILES ${QTNETWORKDLL} DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/data" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
endif(WIN32)

# ** CPack section **

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

  include(InstallRequiredSystemLibraries)

  # Allow CPack to owerwrite CMAKE_INSTALL_PREFIX
  # set(CPACK_SET_DESTDIR ON)

  # On Windows, we build a Zip file with needed content
  if(WIN32)
    set(CPACK_INSTALL_PREFIX ".")
    # set(CPACK_GENERATOR "ZIP")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "multidiagtools")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "multidiagtools")
  endif(WIN32)

  # On Linux, we build a Debian package
  if(UNIX)
    set(CPACK_INSTALL_PREFIX "/usr")
    set(CPACK_GENERATOR "DEB")
  endif(UNIX)

  # Some common variables
  set(CPACK_PACKAGE_DESCRIPTION "Multi Diag Tools library")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi Diag Tools is a library that offers some connectivity to instruments")
  set(CPACK_PACKAGE_VENDOR "Philippe")
  set(CPACK_PACKAGE_CONTACT "Philippe ")
  set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_MICRO}")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION_MAJOR}.${VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION_MAJOR}.${VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  # Some Debian package specific varaibles
  if(UNIX)
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtcore4 (>= 4:4.5.0), libqtgui4 (>= 4:4.5.0), libstdc++6 , libgcc1 , libc6")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  endif(UNIX)

  set(CPACK_COMPONENT_LIB_DISPLAY_NAME "Libraries (runtime)")
  set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Headers (Only needed for devloppers)")
  set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Some little tools")
  # Dependances
  set(CPACK_COMPONENT_DEV_DEPENDS lib)
  set(CPACK_COMPONENT_TOOLS_DEPENDS lib)
  
  set(CPACK_COMPONENTS_ALL lib dev tools)

  # Keep this instruction at last
  include(CPack)

endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
