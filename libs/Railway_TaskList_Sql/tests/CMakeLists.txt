
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
  src/SqlVehicleTypeRepositoryTest.cpp
#   src/ResponseHandler.cpp
)

add_library(mdtRailway_TaskList_SqlTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtRailway_TaskList_SqlTest Railway_TaskList_Sql Application_Core TestLib_Core Sql_Core_SQLite Qt5::Test)

#=============== Helper functions ===========

function(addRailway_TaskList_SqlTest testName)
  set(complteTestName "mdtRailway_TaskList_Sql_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtRailway_TaskList_SqlTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addRailway_TaskList_SqlTest("SqlVehicleTypeRepositoryTest")
