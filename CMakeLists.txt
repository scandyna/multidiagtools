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

# CMake minimal requiered version
cmake_minimum_required(VERSION 3.2.0)

#################################################################################################################################################
# *** Common setup ***
#################################################################################################################################################

# Project
project(multidiagtools)

# If nothing is specified, we build Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

# Project version
set(MDTLIB_VERSION_MAJOR 0)
set(MDTLIB_VERSION_MINOR 3)
set(MDTLIB_VERSION_MICRO 4)

# Library version suffix:
#  Since version 0.2.8, naming convension has changed.
#  The problem was that API is currently unstable and breaks
#  distributed version compatibility.
#  The new binary library name is now: libname-VMAJOR.VMINOR
# This convention is used for:
#  - binary libraries
#  - header directory
#  - binary tools
set(MDTLIB_VERSION_SUFFIX "-${MDTLIB_VERSION_MAJOR}.${MDTLIB_VERSION_MINOR}")

# Config file
configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/config.h"
)
include_directories("${CMAKE_BINARY_DIR}")

# C++ standard
set(CMAKE_CXX_STANDARD 14)

# Compiler warnings options
add_compile_options(-Wall -Wextra)
# add_compile_options(-Werror -Wpedantic)

# Test coverage , NOTE: only gcc ?
# TODO Should be in egbug mode only
#add_compile_options(-ftest-coverage)


# Sanitizer
# NOTE: must be the first option, else undefined references will occur.
#       when using add_compile_options(), it is not at first,
#       so we use CMAKE_CXX_FLAGS (yes, a workaround)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

# Optimisation options:
# By default, CMake will set optimisation for Release, but not for debug
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_compile_options(-O3)
endif()

# On UNIX, we allways add debug symbols (will be stripped and put into separate .dbg files)
if(UNIX)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(-g)
  endif()
endif(UNIX)

# On Windows, we separate debug and release dlls
# if(WIN32)
#   set(CMAKE_DEBUG_POSTFIX d)
# endif(WIN32)

# We want GUI executables on Windows (not console)
if(WIN32)
  set(GUI_TYPE WIN32)
endif(WIN32)

# Additionnal Cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# As moc files are generated in the binary dir, tell CMake to always look for includes there
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#################################################################################################################################################
# *** Helper function (must be placed before add_subdirectory(..) ***
#################################################################################################################################################

# NOTE should be renamed mdtSeparateLibraryDebugSymbols
# NOTE should only do this in UNIX
# Separate debug symbols into a separate .dbg file
function(mdtSeparateDebugSymbols libraryBinName)
  set(completeLibBinName "${CMAKE_SHARED_LIBRARY_PREFIX}${libraryBinName}${MDTLIB_VERSION_SUFFIX}")
  add_custom_command(TARGET "${libraryBinName}${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${completeLibBinName}.so" "${completeLibBinName}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${completeLibBinName}.so"
                    COMMAND objcopy --add-gnu-debuglink="${completeLibBinName}.dbg" "${completeLibBinName}.so"
                    COMMAND chmod -x "${completeLibBinName}.dbg")
  install(FILES "${libraryBinName}${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endfunction()

#################################################################################################################################################
# *** Find common dependencies ***
#################################################################################################################################################

# Thread support
find_package(Threads REQUIRED)

# Enable Qt5 support
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED) # Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

# Get Qwt
find_package(Qwt REQUIRED)
include_directories(${QWT_INCLUDE_DIRS})
get_filename_component(QWT_LIBRARY_DIR ${QWT_LIBRARIES} PATH)
if(WIN32)
  link_directories(${QWT_LIBRARY_DIR})
endif(WIN32)

# Get Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)

#################################################################################################################################################
# *** Librairies section ***
#################################################################################################################################################

# Headers that are used allmost everywhere
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/QtSolutions/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/Application/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/Error/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/ErrorGui/src")

# Headers directories
#include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtcsv")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtdatabase")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtdevice")
include_directories("${PROJECT_SOURCE_DIR}/src/mdterror")
include_directories("${PROJECT_SOURCE_DIR}/src/mdterrorgui")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtport")

# Should be removed once cable list has migrated to libs/CableList
include_directories("${PROJECT_SOURCE_DIR}/libs/CableList/src")

include_directories("${PROJECT_SOURCE_DIR}/src/mdttools")
include_directories("${PROJECT_SOURCE_DIR}/src/mdttools/mdttesttool")
include_directories("${PROJECT_SOURCE_DIR}/src/mdttools/mdttesttool/mdtcablelist")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtutils")
include_directories("${PROJECT_SOURCE_DIR}/src/mdtutilsgui")
include_directories("${PROJECT_SOURCE_DIR}/src/qt-solutions/qtsingleapplication")
# Because some headers are generated during building (config.h, Qt's UIC), we also add path to those
include_directories("${CMAKE_BINARY_DIR}/src")
include_directories("${CMAKE_BINARY_DIR}/src/mdtcsv")
include_directories("${CMAKE_BINARY_DIR}/src/mdtdatabase")
include_directories("${CMAKE_BINARY_DIR}/src/mdtdevice")
include_directories("${CMAKE_BINARY_DIR}/src/mdterror")
include_directories("${CMAKE_BINARY_DIR}/src/mdterrorgui")
include_directories("${CMAKE_BINARY_DIR}/src/mdtport")

# include_directories("${CMAKE_BINARY_DIR}/src/mdtserialport")

include_directories("${CMAKE_BINARY_DIR}/src/mdttools")
include_directories("${CMAKE_BINARY_DIR}/src/mdttools/mdttesttool")
include_directories("${CMAKE_BINARY_DIR}/src/mdttools/mdttesttool/mdtcablelist")
include_directories("${CMAKE_BINARY_DIR}/src/mdtutils")
include_directories("${CMAKE_BINARY_DIR}/src/mdtutilsgui")
include_directories("${CMAKE_BINARY_DIR}/src/qt-solutions/qtsingleapplication")
include_directories("${CMAKE_BINARY_DIR}/tests/mdtutilsgui")


# NOTE: Support Windows pas terminé
if(UNIX)
  include_directories("${PROJECT_SOURCE_DIR}/src/mdtport/linux")
  # include_directories("${PROJECT_SOURCE_DIR}/src/mdtserialport/linux")
endif(UNIX)

# Enable testing
# Must be placed before any add_subdirectory() command, else tests that are defined in sub-directories will be ignored
enable_testing()

# Libraries and headers install directories
if(WIN32)
  set(LIB_DEST_DIR "bin")
  #set(HEADERS_DEST_DIR "include/mdt/${MDTLIB_VERSION_MAJOR}.${MDTLIB_VERSION_MINOR}")
  set(HEADERS_DEST_DIR "include/mdt/mdt${MDTLIB_VERSION_SUFFIX}")
else(WIN32)
  set(LIB_DEST_DIR "lib/${CMAKE_LIBRARY_ARCHITECTURE}")
  #set(HEADERS_DEST_DIR "include/mdt/${MDTLIB_VERSION_MAJOR}.${MDTLIB_VERSION_MINOR}")
  set(HEADERS_DEST_DIR "include/mdt/mdt${MDTLIB_VERSION_SUFFIX}")
endif(WIN32)

# * QtSolutions *
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/QtSolutions")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/qt-solutions")

# * Application *
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Application")

# * Algorithm *
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Algorithm")

# * Expected *
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Expected")

# * mdtutils *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtutils")

# Numeric library
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Numeric")

add_subdirectory("${PROJECT_SOURCE_DIR}/libs/FilterExpression")
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/ItemModel")

# * mdtcsv *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtcsv")

# * mdterror and mdterrorgui *
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Error")
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/ErrorGui")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdterror")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdterrorgui")

# * mdtutilsgui *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtutilsgui")

# * mdtdatabase *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtdatabase")
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/Sql")

# ** Item/View **
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/ItemEditor")
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/ItemEditor_Sql")

# SerialPort
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/SerialPort")

# CableList
add_subdirectory("${PROJECT_SOURCE_DIR}/libs/CableList")

# NOTE: Support Windows pas terminé
if(UNIX)
# * mdtdevice *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtdevice")

# * mdtport *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdtport")

# * mdttools *
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdttools")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdttools/mdttesttool")
#add_subdirectory("${PROJECT_SOURCE_DIR}/src/mdttools/mdttesttool/mdtcablelist")

# *** Tools section (executables) ***
add_subdirectory("${PROJECT_SOURCE_DIR}/tools")

endif(UNIX)

#################################################################################################################################################
# *** Unit tests section ***
#################################################################################################################################################

# ** Unit tests compilation **
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")

# ** Unit tests execution **
# enable_testing()

# * mdtutils *

# mdtAlgorithmsTest
add_test (mdtAlgorithmsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtalgorithmstest)
# mdtMathTest
add_test (mdtMathTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtmathtest)
# mdtErrorTest
# add_test (mdtErrorTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdterrortest)
# mdtErrorGuiTest
# add_test (mdtErrorGuiTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdterrorguitest)
# mdtBufferTest
add_test (mdtBufferTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtbuffertest)
# mdtFrameTest
add_test (mdtFrameTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtframetest)
# mdtFrameCodecTest
add_test (mdtFrameCodecTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtframecodectest)
# mdtCodecTest
add_test (mdtCodecTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtcodectest)
# mdtUicNumberTest
add_test (mdtUicNumberTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtuicnumbertest)
# mdtFileTest
add_test (mdtFileTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtfiletest)
# mdtValueTest
add_test (mdtValueTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtvaluetest)
# mdtIoTest
add_test (mdtIoTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtiotest)
# mdtGraphTest
add_test (mdtGraphTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtgraphtest)
# mdtStateMachineTest
add_test (mdtStateMachineTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutils/mdtstatemachinetest)

# * mdtcsv *

# mdtCsvTest
add_test (mdtCsvTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtcsv/mdtcsvtest)
# mdtSqlCsvTest
add_test (mdtSqlCsvTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtcsv/mdtsqlcsvtest)

# * mdtutilsgui *

# mdtIoWidgetTest
add_test (mdtIoWidgetTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutilsgui/mdtiowidgettest)

# mdtWidgetsTest
add_test (mdtWidgetsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutilsgui/mdtwidgetstest)

# mdtDataTableTest
# add_test (mdtDataTableTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutilsgui/mdtdatatabletest)

# mdtDatabaseTestOld
# add_test (mdtDatabaseTestOld ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutilsgui/mdtdatabasetestold)

# * mdtdatabase *
add_test (mdtClConnectorTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdatabase/mdtclconnectortest)
# add_test (mdtDatabaseTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdatabase/mdtdatabasetest)
# add_test (mdtDatabaseWidgetTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdatabase/mdtdatabasewidgettest)
add_test (mdtSqlCopierTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdatabase/mdtsqlcopiertest)
add_test (mdtSqlApplicationWidgetsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdatabase/mdtsqlapplicationwidgetstest)

# mdtIoWidgetTest
# add_test (mdtIoWidgetTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtutilsgui/mdtiowidgettest)

# * mdtport *

# mdtDeviceAddressTest
add_test (mdtDeviceAddressTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtdeviceaddresstest)

# NOTE: Support Windows pas terminé
# if(UNIX)
#   # mdtPortLockTest
#   add_test (mdtPortLockTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtportlocktest)
#   # mdtPortTest
#   add_test (mdtPortTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtporttest)
#   # mdtPortInfoTest
#   add_test (mdtPortInfoTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtportinfotest)
#   # mdtUsbPortTest
#   add_test (mdtUsbPortTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtusbporttest)
#   # mdtUsbTmcPortTest
#   add_test (mdtUsbTmcPortTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtusbtmcporttest)
#   # mdtTcpSocketTest
#   add_test (mdtTcpSocketTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdttcpsockettest)
#   # mdtPortManagerTest
#   add_test (mdtPortManagerTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtport/mdtportmanagertest)
# 
#   add_test (mdtDeviceTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdtdevice/mdtdevicetest)
# 
#   # mdtTtValueLimitsTest
#   add_test (mdtTtValueLimitsTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdttvaluelimitstest)
#   # mdtTtTestSystemComponentTest
#   add_test (mdtTtTestSystemComponentTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdttestsystemcomponenttest)
#   # mdtTestNodeTest
#   add_test (mdtTestNodeTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdttestnodetest)
#   # mdtTestCableTest
#   add_test (mdtTestToolTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdttestcabletest)
#   # mdtTestToolTest
#   add_test (mdtTestToolTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdttesttooltest)
# 
# 
# endif(UNIX)

# * Tools - mdt cable list *

# mdtClModificationTest
add_test (mdtClModificationTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclmodificationtest)
# mdtClConnectorTest
add_test (mdtClConnectorTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclconnectortest)
# mdtClArticleConnectionTest
add_test (mdtClArticleConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclarticleconnectiontest)
# mdtClUnitConnectionTest
add_test (mdtClUnitConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclunitconnectiontest)

# mdtClLinkTest
add_test (mdtClLinkTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtcllinktest)
# mdtClArticleLinkTest
add_test (mdtClArticleLinkTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclarticlelinktest)
# mdtClAutoConnectionTest
add_test (mdtClAutoConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtclautoconnectiontest)

# mdtCableListTest
add_test (mdtCableListTest ${CMAKE_CURRENT_BINARY_DIR}/tests/mdttools/mdttesttool/mdtcablelisttest)

# mdtClUnitTest
add_test (mdtClUnitTest ${CMAKE_CURRENT_BINARY_DIR}/tools/mdtcablelist/tests/mdtclunittest)

#################################################################################################################################################
# * Translations section *
#################################################################################################################################################

# Create a target to generate qm files
file(GLOB TSFILES "${PROJECT_SOURCE_DIR}/translations/*.ts")
foreach(TSFILE ${TSFILES})
  get_filename_component(TSFILE_ABS "${TSFILE}" ABSOLUTE)
  get_filename_component(QMFILE "${TSFILE_ABS}" NAME_WE)
  set(QMFILE "${PROJECT_SOURCE_DIR}/data/i18n/${QMFILE}.qm")
  add_custom_command(OUTPUT ${QMFILE} COMMAND "${QT_LRELEASE_EXECUTABLE}" ARGS "${TSFILE_ABS}" "-qm" "${QMFILE}" DEPENDS "${TSFILE_ABS}")
  set(QMFILES ${QMFILES} ${QMFILE})
endforeach(TSFILE ${TSFILES})
add_custom_target(qm ALL DEPENDS ${QMFILES})

#################################################################################################################################################
# *** Install section ***
#################################################################################################################################################

# Config file
install(FILES "${PROJECT_SOURCE_DIR}/src/config.h" DESTINATION "${HEADERS_DEST_DIR}" COMPONENT dev)

# Translation files

# Some Linux specific files
if(UNIX)
  install(FILES "${PROJECT_SOURCE_DIR}/dist/debian/etc/udev/rules.d/55-usb-instruments.rules" DESTINATION "/etc/udev/rules.d/" COMPONENT data)
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/data" DESTINATION "/usr/share/mdt/" COMPONENT data)
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
  #install(DIRECTORY "${PROJECT_SOURCE_DIR}/data" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/data" DESTINATION "." COMPONENT data)
  # Qt translations files
  install(FILES "${QT_TRANSLATIONS_DIR}/qt_fr.qm" DESTINATION "data/i18n" COMPONENT lib)
  install(FILES "${QT_TRANSLATIONS_DIR}/qt_de.qm" DESTINATION "data/i18n" COMPONENT lib)
endif(WIN32)

# ** CPack section **

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

  include(InstallRequiredSystemLibraries)

  # Allow CPack to owerwrite CMAKE_INSTALL_PREFIX
  # set(CPACK_SET_DESTDIR ON)

  # On Windows, we build a installer with NSIS
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
    #set(CPACK_GENERATOR "DEB")
  endif(UNIX)

  # Some common variables
  set(CPACK_PACKAGE_DESCRIPTION "Multi Diag Tools library")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi Diag Tools is a library that offers some connectivity to instruments")
  set(CPACK_PACKAGE_VENDOR "Philippe")
  set(CPACK_PACKAGE_CONTACT "Philippe ")
  set(CPACK_PACKAGE_VERSION_MAJOR "${MDTLIB_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${MDTLIB_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${MDTLIB_VERSION_MICRO}")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  set(CPACK_COMPONENT_LIB_DISPLAY_NAME "Libraries (runtime)")
  set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Headers (Only needed for devloppers)")
  set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Some little tools")
  # Dependancies
  set(CPACK_COMPONENT_LIBS_DEPENDS data)
  set(CPACK_COMPONENT_DEV_DEPENDS lib)
  set(CPACK_COMPONENT_TOOLS_DEPENDS lib)
  # Some Debian package specific varaibles
  #if(UNIX)
    #set(CPACK_DEB_COMPONENT_INSTALL ON)
    #set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtcore4 (>= 4:4.7.0), libqtgui4 (>= 4:4.7.0), libstdc++6 , libgcc1 , libc6, libusb (>=1.0)")
    #set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    #set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    #set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    # set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS TRUE)
  #endif(UNIX)

  set(CPACK_COMPONENTS_ALL lib dev tools data)

  # Keep this instruction at last
  include(CPack)

endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

###################################################################################################################################################
#           Debian packages section
###################################################################################################################################################

#### lib package ####
if(UNIX)
  file(WRITE  package_debian_lib.cmake "SET(CPACK_CMAKE_GENERATOR \"Unix Makefiles\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_DEBIAN_PACKAGE_DEPENDS \"libqtcore4 (>= 4:4.7.0), libqtgui4 (>= 4:4.7.0), libstdc++6 , libgcc1 , libc6, libusb-1.0-0 (>=1.0), ${CMAKE_PROJECT_NAME}-data (>= ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_DEBIAN_PACKAGE_PRIORITY \"optional\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_DEBIAN_PACKAGE_SECTION \"libs\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_GENERATOR \"DEB\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_INSTALL_CMAKE_PROJECTS \"${PROJECT_SOURCE_DIR};${CMAKE_PROJECT_NAME};lib;/\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_INSTALL_PREFIX \"/usr\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_CONTACT \"Philippe \")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_DEFAULT_LOCATION \"/\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_DESCRIPTION \"Multi Diag Tools library\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY \"Multi Diag Tools is a library that offers some connectivity to instruments\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_FILE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-lib_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CMAKE_SYSTEM_PROCESSOR}\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-lib\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_VENDOR \"Philippe\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_VERSION_MAJOR \"${CPACK_PACKAGE_VERSION_MAJOR}\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_VERSION_MINOR \"${CPACK_PACKAGE_VERSION_MINOR}\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_PACKAGE_VERSION_PATCH \"${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_RESOURCE_FILE_LICENSE \"${PROJECT_SOURCE_DIR}/COPYING.LESSER\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_RESOURCE_FILE_README \"${PROJECT_SOURCE_DIR}/README\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_SET_DESTDIR \"OFF\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_SYSTEM_NAME \"Linux\")\n")
  file(APPEND package_debian_lib.cmake "SET(CPACK_TOPLEVEL_TAG \"Linux\")\n")
endif(UNIX)

#### dev package ####
if(UNIX)
  file(WRITE  package_debian_dev.cmake "SET(CPACK_CMAKE_GENERATOR \"Unix Makefiles\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_DEBIAN_PACKAGE_DEPENDS \"libqt4-dev (>= 4:4.7.0), make, libusb-1.0-0-dev (>=1.0), libboost-graph-dev, ${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-lib (>= ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_DEBIAN_PACKAGE_PRIORITY \"optional\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_DEBIAN_PACKAGE_SECTION \"libs\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_GENERATOR \"DEB\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_INSTALL_CMAKE_PROJECTS \"${PROJECT_SOURCE_DIR};${CMAKE_PROJECT_NAME};dev;/\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_INSTALL_PREFIX \"/usr\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_CONTACT \"Philippe \")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_DEFAULT_LOCATION \"/\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_DESCRIPTION \"Multi Diag Tools library\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY \"Multi Diag Tools is a library that offers some connectivity to instruments\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_FILE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-dev_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-dev\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_VENDOR \"Philippe\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_VERSION_MAJOR \"${CPACK_PACKAGE_VERSION_MAJOR}\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_VERSION_MINOR \"${CPACK_PACKAGE_VERSION_MINOR}\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_PACKAGE_VERSION_PATCH \"${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_RESOURCE_FILE_LICENSE \"${PROJECT_SOURCE_DIR}/COPYING.LESSER\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_RESOURCE_FILE_README \"${PROJECT_SOURCE_DIR}/README\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_SET_DESTDIR \"OFF\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_SYSTEM_NAME \"Linux\")\n")
  file(APPEND package_debian_dev.cmake "SET(CPACK_TOPLEVEL_TAG \"Linux\")\n")
endif(UNIX)

#### tools package ####
if(UNIX)
  file(WRITE  package_debian_tools.cmake "SET(CPACK_CMAKE_GENERATOR \"Unix Makefiles\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_DEBIAN_PACKAGE_DEPENDS \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-lib (>= ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_DEBIAN_PACKAGE_PRIORITY \"optional\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_DEBIAN_PACKAGE_SECTION \"libs\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_GENERATOR \"DEB\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_INSTALL_CMAKE_PROJECTS \"${PROJECT_SOURCE_DIR};${CMAKE_PROJECT_NAME};tools;/\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_INSTALL_PREFIX \"/usr\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_CONTACT \"Philippe \")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_DEFAULT_LOCATION \"/\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_DESCRIPTION \"Multi Diag Tools library\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY \"Multi Diag Tools is a library that offers some connectivity to instruments\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_FILE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-tools_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CMAKE_SYSTEM_PROCESSOR}\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-tools\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_VENDOR \"Philippe\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_VERSION_MAJOR \"${CPACK_PACKAGE_VERSION_MAJOR}\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_VERSION_MINOR \"${CPACK_PACKAGE_VERSION_MINOR}\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_PACKAGE_VERSION_PATCH \"${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_RESOURCE_FILE_LICENSE \"${PROJECT_SOURCE_DIR}/COPYING.LESSER\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_RESOURCE_FILE_README \"${PROJECT_SOURCE_DIR}/README\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_SET_DESTDIR \"OFF\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_SYSTEM_NAME \"Linux\")\n")
  file(APPEND package_debian_tools.cmake "SET(CPACK_TOPLEVEL_TAG \"Linux\")\n")
endif(UNIX)

#### dbg package ####
if(UNIX)
  file(WRITE  package_debian_dbg.cmake "SET(CPACK_CMAKE_GENERATOR \"Unix Makefiles\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_DEBIAN_PACKAGE_DEPENDS \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-lib (>= ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_DEBIAN_PACKAGE_PRIORITY \"optional\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_DEBIAN_PACKAGE_SECTION \"libs\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_GENERATOR \"DEB\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_INSTALL_CMAKE_PROJECTS \"${PROJECT_SOURCE_DIR};${CMAKE_PROJECT_NAME};dbg;/\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_INSTALL_PREFIX \"/usr\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_CONTACT \"Philippe \")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_DEFAULT_LOCATION \"/\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_DESCRIPTION \"Multi Diag Tools library\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY \"Multi Diag Tools is a library that offers some connectivity to instruments\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_FILE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-dbg_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CMAKE_SYSTEM_PROCESSOR}\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_NAME \"${CMAKE_PROJECT_NAME}${MDTLIB_VERSION_SUFFIX}-dbg\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_VENDOR \"Philippe\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_VERSION_MAJOR \"${CPACK_PACKAGE_VERSION_MAJOR}\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_VERSION_MINOR \"${CPACK_PACKAGE_VERSION_MINOR}\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_PACKAGE_VERSION_PATCH \"${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_RESOURCE_FILE_LICENSE \"${PROJECT_SOURCE_DIR}/COPYING.LESSER\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_RESOURCE_FILE_README \"${PROJECT_SOURCE_DIR}/README\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_SET_DESTDIR \"OFF\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_SYSTEM_NAME \"Linux\")\n")
  file(APPEND package_debian_dbg.cmake "SET(CPACK_TOPLEVEL_TAG \"Linux\")\n")
endif(UNIX)

#### data package ####
if(UNIX)
  file(WRITE  package_debian_data.cmake "SET(CPACK_CMAKE_GENERATOR \"Unix Makefiles\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_DEBIAN_PACKAGE_DEPENDS \"\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_DEBIAN_PACKAGE_PRIORITY \"optional\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_DEBIAN_PACKAGE_SECTION \"libs\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_GENERATOR \"DEB\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_INSTALL_CMAKE_PROJECTS \"${PROJECT_SOURCE_DIR};${CMAKE_PROJECT_NAME};data;/\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_INSTALL_PREFIX \"\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_CONTACT \"Philippe \")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_DEFAULT_LOCATION \"/\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_DESCRIPTION \"Multi Diag Tools library\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY \"Multi Diag Tools is a library that offers some connectivity to instruments\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_FILE_NAME \"${CMAKE_PROJECT_NAME}-data_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CMAKE_SYSTEM_PROCESSOR}\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_NAME \"${CMAKE_PROJECT_NAME}-data\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_VENDOR \"Philippe\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_VERSION_MAJOR \"${CPACK_PACKAGE_VERSION_MAJOR}\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_VERSION_MINOR \"${CPACK_PACKAGE_VERSION_MINOR}\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_PACKAGE_VERSION_PATCH \"${CPACK_PACKAGE_VERSION_PATCH}\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_RESOURCE_FILE_LICENSE \"${PROJECT_SOURCE_DIR}/COPYING.LESSER\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_RESOURCE_FILE_README \"${PROJECT_SOURCE_DIR}/README\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_SET_DESTDIR \"OFF\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_SYSTEM_NAME \"Linux\")\n")
  file(APPEND package_debian_data.cmake "SET(CPACK_TOPLEVEL_TAG \"Linux\")\n")
endif(UNIX)
