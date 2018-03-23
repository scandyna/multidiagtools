
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
  src/SetupWidgetTest.cpp
)

add_library(mdtEntity_WidgetsTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtEntity_WidgetsTest Entity_Widgets Application_Widgets TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addEntity_WidgetsTest testName)
  set(complteTestName "mdtEntity_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtEntity_WidgetsTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addEntity_WidgetsTest("SetupWidgetTest")
addEntity_WidgetsTest("FieldAttributesItemDelegateTest")
