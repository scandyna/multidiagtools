
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtAlgorithTestTest
add_executable(mdtalgorithmtest src/AlgorithmTest.cpp)
target_link_libraries(mdtalgorithmtest Algorithm Application Qt5::Test)
add_test(MdtAlgorithTestTest "${CMAKE_CURRENT_BINARY_DIR}/mdtalgorithmtest")
