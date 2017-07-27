
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= DeployUtils test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtdeployutilstest SHARED ${SOURCE_FILES})
target_link_libraries(mdtdeployutilstest DeployUtils_Core Application Qt5::Test)

#=============== Helper functions ===========

function(addDeployUtilsTest testName)
  set(complteTestName "MdtDeployUtils_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtdeployutilstest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addDeployUtilsTest("PathTest")
addDeployUtilsTest("LibraryTest")
addDeployUtilsTest("LddParserTest")
addDeployUtilsTest("LddWrapperTest")
addDeployUtilsTest("LddDependenciesParserTest")
addDeployUtilsTest("PlatformTest")
addDeployUtilsTest("BinaryDependenciesTest")
addDeployUtilsTest("BinaryDependenciesLddTest")
