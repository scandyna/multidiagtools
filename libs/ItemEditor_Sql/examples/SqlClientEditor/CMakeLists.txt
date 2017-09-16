
# List of source files
set(SOURCE_FILES
    src/main.cpp
    src/Client.cpp
    src/Address.cpp
    src/ClientEditorSchema.cpp
    src/SqlClientWidget.cpp
    src/SqlAddressWidget.cpp
    src/SqlClientEditorMainWidget.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets)

add_executable(sqlclienteditor ${SOURCE_FILES})
target_link_libraries(sqlclienteditor ItemEditor_Sql Application_Widgets Qt5::Widgets)
