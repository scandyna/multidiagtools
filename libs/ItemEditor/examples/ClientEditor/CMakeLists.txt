
# List of source files
set(SOURCE_FILES
    src/main.cpp
    src/ClientModel.cpp
    src/ClientWidget.cpp
    src/ClientListWidget.cpp
    src/AddressModel.cpp
    src/AddressWidget.cpp
    src/MainWidget.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets)

add_executable(clienteditor ${SOURCE_FILES})
target_link_libraries(clienteditor ItemEditor Application Qt5::Widgets)
