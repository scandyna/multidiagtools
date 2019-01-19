
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Relection_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtReflection_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtReflection_CoreTest Reflection_Core Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addReflection_CoreTest testName)
  set(complteTestName "MdtReflection_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtReflection_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
  target_compile_definitions(${executableName} PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addReflection_CoreTest("Sandbox")

addReflection_CoreTest("ReflectFieldTest")
addReflection_CoreTest("ReflectStructTest")
addReflection_CoreTest("TypeTraitsTest")
addReflection_CoreTest("FieldAttributesTest")
addReflection_CoreTest("FieldTest")
addReflection_CoreTest("StructTest")
addReflection_CoreTest("PrimaryKeyTest")
addReflection_CoreTest("UniqueConstraintTest")
