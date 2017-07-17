
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtAlgorithTestTest
add_executable(mdtalgorithmtest src/AlgorithmTest.cpp)
target_link_libraries(mdtalgorithmtest Algorithm Application Qt5::Test)
add_test(NAME MdtAlgorithTestTest COMMAND mdtalgorithmtest)
# set_tests_properties(MdtAlgorithTestTest PROPERTIES ENVIRONMENT "PATH=/home/philippe/opt/build/cross/mxe/usr/i686-w64-mingw32.shared.posix/bin/;/home/philippe/opt/build/cross/mxe/usr/i686-w64-mingw32.shared.posix/qt5/bin/;$ENV{PATH}")
# 
# get_test_property(MdtAlgorithTestTest ENVIRONMENT prop)
# message("----------- TST prop: ${prop}")
# 
# add_test(MdtAlgorithTestTest "${CMAKE_CURRENT_BINARY_DIR}/mdtalgorithmtest")
