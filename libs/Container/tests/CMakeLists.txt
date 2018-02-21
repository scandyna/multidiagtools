
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Container test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtContainerTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtContainerTest Container Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addContainerTest testName)
  set(complteTestName "mdtContainer_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtContainerTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addContainerTest("StlContainerTest")
addContainerTest("StlContainerBenchmark")
