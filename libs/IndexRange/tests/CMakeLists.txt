
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= IndexRange test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtIndexRangeTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtIndexRangeTest IndexRange Application_Core TestLib_Container Qt5::Test)

#=============== Helper functions ===========

function(addIndexRangeTest testName)
  set(complteTestName "mdtIndexRange_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtIndexRangeTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addIndexRangeTest("RowColumnRangeTest")
