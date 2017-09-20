
enable_testing()

find_package(Qt5 COMPONENTS Test)

# Target MdtExpectedTest
add_executable(mdtexpectedtest src/ExpectedTest.cpp)
add_test(NAME MdtExpectedTest COMMAND mdtexpectedtest)
target_link_libraries(mdtexpectedtest Expected Application_Core Qt5::Test)

# Clang warns about explicit move that prevents copy elision.
# For this test case, we need explicit move to check our code
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(mdtexpectedtest PRIVATE -Wno-pessimizing-move)
endif()
