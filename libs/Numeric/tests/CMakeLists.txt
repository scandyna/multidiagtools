
enable_testing()

find_package(Qt5 COMPONENTS Test Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#=============== Numeric test library =====

set(SOURCES_FILES
    src/TestBase.cpp
)

add_library(mdtNumericTest SHARED ${SOURCES_FILES})
target_link_libraries(mdtNumericTest Numeric Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addNumericTest testName)
  set(complteTestName "MdtNumeric_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtNumericTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
endfunction()

#================= Tests ====================

addNumericTest("DoubleTest")
addNumericTest("PhysicsTypesTest")
addNumericTest("IntegralRangeTest")
addNumericTest("IntegralRangeBenchmark")
addNumericTest("IntegralUniqueIdTest")
