

set(SQLITE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/sqlite")

add_subdirectory(src)
add_subdirectory(tests)
# add_subdirectory(translations)
add_subdirectory(plugins)
