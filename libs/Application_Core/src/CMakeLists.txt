
find_package(Qt5 COMPONENTS Core)

configure_file(
  Mdt/Version.h.in
  Version.h
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  Mdt/StandardPaths.cpp
  Mdt/CoreApplicationImpl.cpp
  Mdt/CoreApplication.cpp
  Mdt/AbstractConsoleApplicationMainFunction.cpp
)

mdt_add_library(
  NAME Application_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Qt5::Core
)
target_compile_definitions(Application_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Application_Core
  DESCRIPTION "Helper to create main (init Error subsystem, load translations, etc..)"
)
