
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Translation_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtFileSystem_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtFileSystem_CoreTest FileSystem_Core Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addFileSystem_CoreTest testName)
  set(complteTestName "mdtFileSystem_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtFileSystem_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addFileSystem_CoreTest("PathTest")
