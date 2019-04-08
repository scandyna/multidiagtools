
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
)

add_library(mdtAsyncTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtAsyncTest Async Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addAsyncTest testName)
  set(complteTestName "MdtAsync_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtAsyncTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
  target_compile_definitions(${executableName} PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addAsyncTest("WaitTest")
addAsyncTest("WaitBenchmark")
