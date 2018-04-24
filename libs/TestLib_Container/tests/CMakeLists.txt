
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= TestLib_Container test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtTestLib_ContainerTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtTestLib_ContainerTest TestLib_Container Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addTestLib_ContainerTest testName)
  set(complteTestName "mdtTestLib_Container_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtTestLib_ContainerTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addTestLib_ContainerTest("CompareRowColumnListTest")
