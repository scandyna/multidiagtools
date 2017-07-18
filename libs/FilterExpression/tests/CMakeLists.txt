
enable_testing()

find_package(
  Qt5
  COMPONENTS Test
)

#=============== Helper functions ===========

# Add a test for this FilterExpression lib
function(addFilterExpressionTest testName)
  set(complteTestName "MdtFilterExpression${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} FilterExpression Application Qt5::Test)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addFilterExpressionTest("LikeExpressionTest")
addFilterExpressionTest("FilterExpressionTest")
