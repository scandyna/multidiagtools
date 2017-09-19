
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  Mdt/SingleCoreApplication.cpp
)

mdt_add_library(
  NAME Application_Core_Single
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Application_Core QtSingleApplication_Core Qt5::Core
)
target_compile_definitions(Application_Core_Single PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Application_Core_Single
  DESCRIPTION "Helper, using QtSingleCoreApplication, to create a single instance application main"
)
