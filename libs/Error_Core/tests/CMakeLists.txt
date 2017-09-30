
enable_testing()

find_package(Qt5 COMPONENTS Test)

#=============== Helper functions ===========

function(addErrorCoreTest testName)
  set(complteTestName "MdtError_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} Error_Core Qt5::Test)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

# Tests
addErrorCoreTest("ErrorTest")
addErrorCoreTest("ErrorLoggerTest")

# # Target MdtErrorTest
# add_executable(mdterrortest src/ErrorTest.cpp)
# target_link_libraries(mdterrortest Error_Core)
# target_link_libraries(mdterrortest Qt5::Test)
# add_test(NAME MdtErrorTest COMMAND mdterrortest)
