
enable_testing()

find_package(Qt5 COMPONENTS Test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#=============== MemoryStore test library =====

set(SOURCES_FILES
    src/TestBase.cpp
)

add_library(mdtMemoryStoreTest SHARED ${SOURCES_FILES})
target_link_libraries(mdtMemoryStoreTest MemoryStore Numeric Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addMemoryStoreTest testName)
  set(complteTestName "MdtMemoryStore_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtMemoryStoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
endfunction()

#================= Tests ====================

addMemoryStoreTest("TableTest")
