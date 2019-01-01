
enable_testing()

find_package(Qt5 COMPONENTS Test Sql)

#================= test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
)

add_library(mdtSql_ReflectionTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtSql_ReflectionTest Sql_Core Sql_Core_SQLite Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addSql_ReflectionTest testName)
  set(complteTestName "MdtSql_Reflection_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtSql_ReflectionTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addSql_ReflectionTest("ReflectionTest")
