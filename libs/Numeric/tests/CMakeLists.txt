
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Tests ====================

# MdtNumericDoubleTest
add_executable(mdtnumericdoubletest src/DoubleTest.cpp)
target_link_libraries(mdtnumericdoubletest Numeric Application Qt5::Test)
add_test(NAME MdtNumericDoubleTest COMMAND mdtnumericdoubletest)

# MdtNumericPhysicsTypesTest
add_executable(mdtnumericphysicstypestest src/PhysicsTypesTest.cpp)
target_link_libraries(mdtnumericphysicstypestest Numeric Application Qt5::Test)
add_test(NAME MdtNumericPhysicsTypesTest COMMAND mdtnumericphysicstypestest)
# add_test(MdtNumericPhysicsTypesTest ${CMAKE_CURRENT_BINARY_DIR}/mdtnumericphysicstypestest)
