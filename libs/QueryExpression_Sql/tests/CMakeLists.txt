
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= QueryExpression_Sql test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtQueryExpression_SqlTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtQueryExpression_SqlTest QueryExpression_Sql Application_Core Entity_Core Entity_Sql TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addQueryExpression_SqlTest testName)
  set(complteTestName "mdtQueryExpression_Sql_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtQueryExpression_SqlTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addQueryExpression_SqlTest("SqlTransformTest")
addQueryExpression_SqlTest("SelectQueryTest")
