
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= QueryExpression_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtQueryExpression_ReflectionTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtQueryExpression_ReflectionTest QueryExpression_Reflection Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addQueryExpression_ReflectionTest testName)
  set(complteTestName "mdtQueryExpression_Reflection_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtQueryExpression_ReflectionTest)
  target_compile_definitions(${executableName} PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addQueryExpression_ReflectionTest("QueryFieldTest")
addQueryExpression_ReflectionTest("SelectStatementTest")
