
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtEntity_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtEntity_CoreTest Entity_Core Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addEntity_CoreTest testName)
  set(complteTestName "mdtEntity_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtEntity_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addEntity_CoreTest("DefTest")
addEntity_CoreTest("FieldTest")
addEntity_CoreTest("ReflectionTest")
