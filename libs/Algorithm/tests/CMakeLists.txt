
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtAlgorithTest
add_executable(mdtalgorithmtest src/AlgorithmTest.cpp)
target_link_libraries(mdtalgorithmtest Algorithm Application_Core Qt5::Test)
add_test(NAME MdtAlgorithTest COMMAND mdtalgorithmtest)

# Target MdtAlgorithBenchmark
add_executable(mdtalgorithmbenchmark src/AlgorithmBenchmark.cpp)
target_link_libraries(mdtalgorithmbenchmark Algorithm Application_Core Qt5::Test)
add_test(NAME MdtAlgorithBenchmark COMMAND mdtalgorithmbenchmark)
