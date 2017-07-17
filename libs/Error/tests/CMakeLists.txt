
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtErrorTest
add_executable(mdterrortest src/ErrorTest.cpp)
target_link_libraries(mdterrortest Error)
target_link_libraries(mdterrortest Qt5::Test)
add_test(NAME MdtErrorTest COMMAND mdterrortest)
# add_test(MdtErrorTest "${CMAKE_CURRENT_BINARY_DIR}/mdterrortest")
