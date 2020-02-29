
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
  src/MemoryVehicleTypeRepository.cpp
#   src/ResponseHandler.cpp
)

add_library(mdtRailway_TaskList_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtRailway_TaskList_CoreTest Railway_TaskList_Core Railway_TaskList_TestLib MemoryStore Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addRailway_TaskList_CoreTest testName)
  set(complteTestName "mdtRailway_TaskList_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtRailway_TaskList_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addRailway_TaskList_CoreTest("MemoryVehicleTypeRepositoryTest")
addRailway_TaskList_CoreTest("GetAllVehicleTypesTest")
addRailway_TaskList_CoreTest("EditVehicleTypeTableModelTest")
