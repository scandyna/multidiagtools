
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtRailway_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtRailway_CoreTest Railway_Core Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addRailway_CoreTest testName)
  set(complteTestName "mdtRailway_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtRailway_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addRailway_CoreTest("VehicleTypeTest")
addRailway_CoreTest("CreateVehicleTypeClassTest")
addRailway_CoreTest("UpdateVehicleTypeClassTest")
addRailway_CoreTest("CreateVehicleTypeTest")
