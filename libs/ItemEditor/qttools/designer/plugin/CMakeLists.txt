
set(SOURCE_FILES
  WidgetMapperWidgetPlugin.cpp
  ItemEditorPlugins.cpp
)

mdt_add_qt_designer_plugin_library(
  PLUGIN_NAME ItemEditorQtDesignerPlugin
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES ItemEditor Qt5::Widgets
)

mdt_set_library_description(
  NAME ItemModel
  DESCRIPTION "ItemEditor Qt designer plugin."
)

# 
# # CMake project file for Multi Diag Tools
# project(qtdesigner_plugins)
# 
# find_package(Qt5Designer REQUIRED)
# 
# 
# # Headers directories
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../src")
# 
# # Plugin library name
# set(PLUGIN_LIBRARY_NAME "ItemEditorQtDesignerPlugins")
# 
# # Plugin library technical names
# string(TOLOWER ${PLUGIN_LIBRARY_NAME} PLUGIN_LIBRARY_NAME_LC)
# set(PLUGIN_LIBRARY_BIN_NAME "mdt${PLUGIN_LIBRARY_NAME_LC}")
# # Add plugin library
# add_library(${PLUGIN_LIBRARY_BIN_NAME} SHARED ${SOURCE_FILES})
# target_compile_definitions(${PLUGIN_LIBRARY_BIN_NAME} PRIVATE -DQT_PLUGIN)
# target_compile_definitions(${PLUGIN_LIBRARY_BIN_NAME} PRIVATE -DQT_NO_DEBUG)
# target_compile_definitions(${PLUGIN_LIBRARY_BIN_NAME} PRIVATE -DQT_SHARED)
# target_compile_definitions(${PLUGIN_LIBRARY_BIN_NAME} PRIVATE -DQTDESIGNER_EXPORT_WIDGETS)
# target_link_libraries(${PLUGIN_LIBRARY_BIN_NAME} "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(${PLUGIN_LIBRARY_BIN_NAME} Qt5::Designer)
# 
# # Strip library and put debug symbols into separate file
# if(UNIX)
#   add_custom_command(TARGET "${PLUGIN_LIBRARY_BIN_NAME}"
#                     POST_BUILD
#                     COMMAND objcopy --only-keep-debug "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.so" "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.dbg"
#                     COMMAND strip --strip-debug --strip-unneeded "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.so"
#                     COMMAND objcopy --add-gnu-debuglink="${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.dbg" "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.so"
#                     COMMAND chmod -x "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_LIBRARY_BIN_NAME}.dbg")
#   install(FILES "${PLUGIN_LIBRARY_BIN_NAME}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
# endif(UNIX)
