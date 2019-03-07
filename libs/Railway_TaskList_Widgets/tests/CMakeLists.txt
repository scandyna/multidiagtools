
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtRailway_TaskList_WidgetsTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtRailway_TaskList_WidgetsTest Railway_TaskList_Widgets Railway_TaskList_Sql Sql_Widgets Sql_Core_SQLite Application_Widgets TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addRailway_TaskList_WidgetsTest testName)
  set(complteTestName "mdtRailway_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtRailway_TaskList_WidgetsTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addRailway_TaskList_WidgetsTest("EditVehicleTypeTest")
