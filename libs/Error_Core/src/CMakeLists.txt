
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    Mdt/Error.cpp
    Mdt/ErrorCode.cpp
    Mdt/ErrorQProcess.cpp
    Mdt/ErrorLogger/Logger.cpp
    Mdt/ErrorLogger/Backend.cpp
    Mdt/ErrorLogger/ConsoleBackend.cpp
    Mdt/ErrorLogger/FileBackend.cpp
    Mdt/ErrorLogger/FormatEngine.cpp
    Mdt/ErrorLogger/FileBackendFormatEngine.cpp
    Mdt/Assert.cpp
)

mdt_add_library(
  NAME Error_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core Threads::Threads
)
target_compile_definitions(Error_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Error_Core
  DESCRIPTION "Error helps for error messages passing, displaying and logging."
)
