
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtRailway_WidgetsTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtRailway_WidgetsTest Railway_Widgets Sql_Widgets Application_Widgets TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addRailway_WidgetsTest testName)
  set(complteTestName "mdtRailway_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtRailway_WidgetsTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addRailway_WidgetsTest("VehicleTypeTest")
