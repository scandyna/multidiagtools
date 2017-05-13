# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= PlainText test library ====================

set(SOURCES_FILES
  src/TestBase.cpp
)

add_library(mdtplaintexttest SHARED ${SOURCES_FILES})
target_link_libraries(mdtplaintexttest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtplaintexttest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtplaintexttest Qt5::Test)

#=============== Helper functions ===========

function(addPlainTextTest testName)
  set(complteTestName "MdtPlainText_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "mdtplaintexttest")
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addPlainTextTest("FileInputIteratorTest")
