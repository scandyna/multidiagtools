
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= DeployUtils test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtdeployutilstest SHARED ${SOURCE_FILES})
target_link_libraries(mdtdeployutilstest PlainTextTestUtils DeployUtils_Core Application_Core Qt5::Test)

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
target_compile_definitions(mdtdeployutils_binarydependenciestest PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")
if(WIN32)
  addDeployUtilsTest("BinaryDependenciesTestWindows")
  target_compile_definitions(mdtdeployutils_binarydependenciestestwindows PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")
endif()
addDeployUtilsTest("BinaryDependenciesLddTest")
addDeployUtilsTest("BinaryDependenciesObjdumpTest")
addDeployUtilsTest("FileCopierTest")
addDeployUtilsTest("QtLibraryTest")
# Tell the test where to serch Qt
# On a Windows machine build, use PATH
# which is set by Qt srcipt
if( (NOT WIN32) OR (WIN32 AND CMAKE_CROSSCOMPILING) )
  target_compile_definitions(mdtdeployutils_qtlibrarytest PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")
endif()
if(UNIX)
  addDeployUtilsTest("QtLibraryTestLinux")
endif()
if(WIN32)
  addDeployUtilsTest("QtLibraryTestWindows")
  target_compile_definitions(mdtdeployutils_qtlibrarytestwindows PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")
  target_link_libraries(mdtdeployutils_qtlibrarytestwindows Application_Widgets)
endif()
addDeployUtilsTest("RPathTest")
if(UNIX AND NOT APPLE)
  addDeployUtilsTest("RPathTestLinux")
endif()
addDeployUtilsTest("QmFileNameTest")
addDeployUtilsTest("QmFileNameBenchmark")
addDeployUtilsTest("TranslationInfoTest")
addDeployUtilsTest("FindTranslationTest")
