
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    Mdt/Error.cpp
    Mdt/ErrorQProcess.cpp
    Mdt/ErrorLogger/Logger.cpp
    Mdt/ErrorLogger/Backend.cpp
    Mdt/ErrorLogger/ConsoleBackend.cpp
    Mdt/ErrorLogger/FileBackend.cpp
)

mdt_add_library(
  NAME Error
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core Threads::Threads
)
mdt_set_library_description(
  NAME Error
  DESCRIPTION "Error helps for error messages passing, displaying and logging."
)
