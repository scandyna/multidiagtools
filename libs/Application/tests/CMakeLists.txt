
enable_testing()

find_package(Qt5
  COMPONENTS Test
)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Target MdtApplicationTest
add_executable(mdtapplicationtest src/ApplicationTest.cpp)
target_link_libraries(mdtapplicationtest Application)
target_link_libraries(mdtapplicationtest Qt5::Test)
add_test(NAME MdtApplicationTest COMMAND mdtapplicationtest)
# add_test(MdtApplicationTest "${CMAKE_CURRENT_BINARY_DIR}/mdtapplicationtest")
