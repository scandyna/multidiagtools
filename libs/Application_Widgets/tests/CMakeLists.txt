
enable_testing()

find_package(Qt5 COMPONENTS Test)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

#================= Application test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtapplicationtestlib SHARED ${SOURCE_FILES})
target_link_libraries(mdtapplicationtestlib Application_Widgets Qt5::Test)

#=============== Helper functions ===========

function(addApplicationTest testName)
  set(complteTestName "MdtApplication_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtapplicationtestlib)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addApplicationTest("ApplicationImplTest")
addApplicationTest("ApplicationTest")
