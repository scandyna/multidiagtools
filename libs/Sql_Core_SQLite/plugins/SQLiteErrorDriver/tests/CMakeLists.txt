# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test)

add_executable(SQLiteErrorDriverPluginTest src/SQLiteErrorDriverPluginTest.cpp ../src/Mdt/Sql/SQLiteErrorDriver.cpp)
target_link_libraries(SQLiteErrorDriverPluginTest Sql_Core_SQLite Qt5::Test)
target_include_directories(SQLiteErrorDriverPluginTest PRIVATE ../src "${SQLITE_SOURCE_DIR}")
add_test(NAME SQLiteErrorDriverPluginTest COMMAND SQLiteErrorDriverPluginTest)
