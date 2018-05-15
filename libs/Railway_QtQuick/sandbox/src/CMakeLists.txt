
find_package(Qt5 COMPONENTS Gui Qml Quick)

set(CMAKE_AUTORCC ON)

add_executable(mdt_railway_qtquick_sandbox "main.cpp" "qml.qrc")
target_link_libraries(mdt_railway_qtquick_sandbox Qt5::Gui Qt5::Qml Qt5::Quick Railway_Core)
