
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Relection_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtReflection_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtReflection_CoreTest  Application_Core Qt5::Test)
# Reflection_Core

#=============== Helper functions ===========

function(addReflection_CoreTest testName)
  set(complteTestName "MdtReflection_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtReflection_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addReflection_CoreTest("Sandbox")
