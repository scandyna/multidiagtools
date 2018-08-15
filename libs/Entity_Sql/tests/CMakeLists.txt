
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Sql test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtEntity_SqlTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtEntity_SqlTest Entity_Sql Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addEntity_SqlTest testName)
  set(complteTestName "mdtEntity_Sql_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtEntity_SqlTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addEntity_SqlTest("FieldTest")
addEntity_SqlTest("PrimaryKeyTest")
addEntity_SqlTest("ForeignKeyTest")
addEntity_SqlTest("TableTest")
addEntity_SqlTest("SelectStatementTest")
addEntity_SqlTest("DataTest")
addEntity_SqlTest("EntityRepositoryTest")
