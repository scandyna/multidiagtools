
set(SOURCE_FILES
#   src/Client.cpp
#   src/ClientEntityExample.cpp
  src/main.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core)

# On Windows, we want a GUI executable, not a console
if(WIN32)
  add_executable(clientEntitySqlExample WIN32 ${SOURCE_FILES})
else()
  add_executable(clientEntitySqlExample ${SOURCE_FILES})
endif()
target_link_libraries(clientEntitySqlExample Entity_Sql Qt5::Core)
