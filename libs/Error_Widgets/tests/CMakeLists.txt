
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtError_WidgetsTest
add_executable(mdterror_widgetstest src/ErrorWidgetsTest.cpp)
target_link_libraries(mdterror_widgetstest Error_Widgets Application Qt5::Test)
add_test(MdtError_WidgetsTest "${CMAKE_CURRENT_BINARY_DIR}/mdterror_widgetstest")
