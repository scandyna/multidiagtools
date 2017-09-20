
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= PlainText test utils library - Can be used by other libraries ====================

set(SOURCE_FILES
  Mdt/PlainText/TestUtils.cpp
)

add_library(PlainTextTestUtils SHARED ${SOURCE_FILES})
target_link_libraries(PlainTextTestUtils PlainText_Core Qt5::Test)
target_include_directories(PlainTextTestUtils PUBLIC
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                          $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
