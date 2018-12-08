
find_package(Qt5 COMPONENTS Core)

set(SOURCE_FILES
    Mdt/Assert.cpp
)

mdt_add_library(
  NAME Assert
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core
)
target_compile_definitions(Assert PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Assert
  DESCRIPTION "Provide MDT_ASSERT() macro similar to Q_ASSERT() but supports constexpr"
)
