
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtExpectedTest
add_executable(mdtexpectedtest src/ExpectedTest.cpp)
add_test(MdtExpectedTest ${CMAKE_CURRENT_BINARY_DIR}/mdtexpectedtest)
target_link_libraries(mdtexpectedtest Expected Application Qt5::Test)
