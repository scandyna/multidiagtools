
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Mdt::LibraryInfo test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
)

add_library(mdtLibraryInfo_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtLibraryInfo_CoreTest LibraryInfo_Core Application_Core Qt5::Test)


if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  get_target_property(LibraryInfoBinaryDir LibraryInfo_Core RUNTIME_OUTPUT_DIRECTORY)
else()
  get_target_property(LibraryInfoBinaryDir LibraryInfo_Core BINARY_DIR)
endif()
target_compile_definitions(mdtLibraryInfo_CoreTest PUBLIC MDT_LIBRARY_INFO_BIN_DIR="${LibraryInfoBinaryDir}" CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

message("LibraryInfoBinaryDir: ${LibraryInfoBinaryDir}")

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addLibraryInfoCoreTest testName)
  set(complteTestName "MdtLibraryInfo_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtLibraryInfo_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addLibraryInfoCoreTest("LibraryInfoTest")

