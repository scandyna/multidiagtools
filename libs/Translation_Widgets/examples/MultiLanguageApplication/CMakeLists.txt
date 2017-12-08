
set(SOURCE_FILES
  src/MultiLanguageMainWindow.cpp
  src/main.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets)

# On Windows, we want a GUI executable, not a console
if(WIN32)
  add_executable(multiLanguageApplication WIN32 ${SOURCE_FILES})
else()
  add_executable(multiLanguageApplication ${SOURCE_FILES})
endif()
target_link_libraries(multiLanguageApplication Translation_Widgets Qt5::Widgets)

add_subdirectory(translations)
