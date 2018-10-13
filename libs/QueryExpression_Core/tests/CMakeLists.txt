
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= QueryExpression_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtQueryExpression_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtQueryExpression_CoreTest QueryExpression_Core Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addQueryExpression_CoreTest testName)
  set(complteTestName "mdtQueryExpression_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtQueryExpression_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addQueryExpression_CoreTest("ExpressionGrammarTest")
addQueryExpression_CoreTest("JoinClauseTest")
addQueryExpression_CoreTest("SelectStatementTest")
addQueryExpression_CoreTest("SelectStatementBenchmark")
addQueryExpression_CoreTest("SelectQueryTest")
addQueryExpression_CoreTest("SelectFieldTest")
addQueryExpression_CoreTest("SelectFieldListTest")
addQueryExpression_CoreTest("LikeExpressionTest")
addQueryExpression_CoreTest("LikeExpressionBenchmark")
addQueryExpression_CoreTest("ExpressionTreeTest")
