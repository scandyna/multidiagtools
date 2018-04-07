
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Entity_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtEntity_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtEntity_CoreTest Entity_Core Application_Core TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addEntity_CoreTest testName)
  set(complteTestName "mdtEntity_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtEntity_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addEntity_CoreTest("FieldAttributesTest")
addEntity_CoreTest("FieldDefTest")
addEntity_CoreTest("TypeTraitsTest")
addEntity_CoreTest("DefTest")
addEntity_CoreTest("ValueAtTest")
addEntity_CoreTest("FieldAtTest")
addEntity_CoreTest("UniqueIdTest")
addEntity_CoreTest("DataTemplateTest")
addEntity_CoreTest("DataListTest")
addEntity_CoreTest("DataListItemModelTest")
addEntity_CoreTest("FieldDataValidatorTest")
addEntity_CoreTest("PrimaryKeyTest")
addEntity_CoreTest("ItemModelPrimaryKeyTest")
addEntity_CoreTest("ForeignKeyTest")
addEntity_CoreTest("ItemModelForeignKeyTest")
addEntity_CoreTest("RelationTest")
addEntity_CoreTest("ItemModelRelationTest")
addEntity_CoreTest("TableModelRecordAdapterTest")
addEntity_CoreTest("TableModelTest")
addEntity_CoreTest("AbstractCachedRepositoryTest")
addEntity_CoreTest("RepositoryHandleTest")
addEntity_CoreTest("StlTableProxyIteratorTest")
addEntity_CoreTest("CachedRepositoryStlTableProxyTest")
# addEntity_CoreTest("StlTableProxyTest")

# Clang warns about explicit move that prevents copy elision.
# For this test case, we need explicit move to check our code
# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   target_compile_options(mdtexpectedtest PRIVATE -Wno-pessimizing-move)
# endif()
