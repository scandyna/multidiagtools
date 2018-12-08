
enable_testing()

find_package(Qt5 COMPONENTS Test)

#=============== Helper functions ===========

function(addAssertTest testName)
  set(complteTestName "MdtAssert_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} Assert Qt5::Test)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

# Tests
addAssertTest("AssertTest")
