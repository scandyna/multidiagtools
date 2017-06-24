
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= PlainText test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtplaintexttest SHARED ${SOURCE_FILES})
target_link_libraries(mdtplaintexttest PlainText_Core Application Qt5::Test)

#=============== Helper functions ===========

function(addPlainTextTest testName)
  set(complteTestName "MdtPlainText_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtplaintexttest)
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addPlainTextTest("FileInputIteratorTest")
addPlainTextTest("FileMultiPassIteratorTest")
addPlainTextTest("StringIteratorTest")
