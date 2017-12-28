
set(SOURCE_FILES
  src/ArticleDef.cpp
  src/ArticleData.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core)

add_library(articleEntityExampleLib SHARED ${SOURCE_FILES})
target_link_libraries(articleEntityExampleLib Entity_Core Qt5::Core)
# Generate Export header (mainly required on Windows for dll exports)
generate_export_header(
  articleEntityExampleLib
  EXPORT_FILE_NAME "ArticleEntityExampleExport.h"
)
target_include_directories(articleEntityExampleLib PUBLIC
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# On Windows, we want a GUI executable, not a console
if(WIN32)
  add_executable(articleEntityExample WIN32 "src/main.cpp")
else()
  add_executable(articleEntityExample "src/main.cpp")
endif()
target_link_libraries(articleEntityExample articleEntityExampleLib)
