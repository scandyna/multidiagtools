
enable_testing()

find_package(Qt5
  COMPONENTS Test
)

# Target MdtError_WidgetsTest
add_executable(mdterror_widgetstest src/ErrorWidgetsTest.cpp)
target_link_libraries(mdterror_widgetstest Error_Widgets Application Qt5::Test)
add_test(NAME MdtError_WidgetsTest COMMAND mdterror_widgetstest)
