
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= DeployUtils test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtdeployutilstest SHARED ${SOURCE_FILES})
target_link_libraries(mdtdeployutilstest PlainTextTestUtils DeployUtils_Core Application Qt5::Test)

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
addDeployUtilsTest("LibraryTreeTest")
addDeployUtilsTest("LddParserTest")
addDeployUtilsTest("LddWrapperTest")
addDeployUtilsTest("ObjdumpWrapperTest")
addDeployUtilsTest("LddDependenciesParserTest")
addDeployUtilsTest("ObjdumpDependenciesParserTest")
addDeployUtilsTest("ObjdumpBinaryFormatParserTest")
addDeployUtilsTest("BinaryFormatTest")
addDeployUtilsTest("PlatformTest")
addDeployUtilsTest("BinaryDependenciesTest")
addDeployUtilsTest("BinaryDependenciesLddTest")
addDeployUtilsTest("BinaryDependenciesObjdumpTest")
addDeployUtilsTest("FileCopierTest")
addDeployUtilsTest("QtLibraryTest")
target_compile_definitions(mdtdeployutils_qtlibrarytest PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")
