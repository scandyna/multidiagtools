# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#=============== ItemModel test library =====

set(SOURCES_FILES
    src/TestBase.cpp
)

add_library(mdtitemmodeltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemmodeltest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemmodeltest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemmodeltest Qt5::Test)

#=============== Qt5 item model test library =====

set(QT5_SOURCES_FILES
    qt5/qtdynamictreemodel.cpp
    qt5/qtmodeltest.cpp
)

add_library(mdtitemmodelqt5test SHARED ${QT5_SOURCES_FILES})
target_link_libraries(mdtitemmodelqt5test Qt5::Widgets)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/qt5")

#=============== Helper functions ===========

# Add a test for this ItemModel lib
function(addItemModelTest testName)
  set(complteTestName "MdtItemModel${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "mdtitemmodeltest")
  target_link_libraries(${executableName} "mdtitemmodelqt5test")
#   target_link_libraries(${executableName} "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
#   target_link_libraries(${executableName} "mdtapplication${MDTLIB_VERSION_SUFFIX}")
#   target_link_libraries(${executableName} Qt5::Test)
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
endfunction()

#================= Tests ====================

addItemModelTest("qt5_tst_modeltest")
addItemModelTest("VariantTableModelTest")
addItemModelTest("RangeTest")
addItemModelTest("FormatMapTest")
addItemModelTest("SortFilterProxyModelTest")
addItemModelTest("FilterExpressionTest")
addItemModelTest("FilterProxyModelTest")
addItemModelTest("RelationFilterProxyModelTest")
addItemModelTest("SortProxyModelTest")
addItemModelTest("FormatProxyModelTest")
addItemModelTest("ProxyModelContainerTest")
addItemModelTest("KeyTest")
addItemModelTest("RelationKeyTest")
addItemModelTest("PkFkProxyModelTest")
