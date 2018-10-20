
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Translation_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtTestLib_WidgetsTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtTestLib_WidgetsTest TestLib_Widgets ItemModel Application_Widgets Qt5::Test)

#=============== Helper functions ===========

function(addTestLib_WidgetsTest testName)
  set(complteTestName "mdtTestLib_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtTestLib_WidgetsTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addTestLib_WidgetsTest("WidgetsTest")
