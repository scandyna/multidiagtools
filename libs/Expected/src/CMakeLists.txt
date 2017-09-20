
set(SOURCE_FILES
    Mdt/Expected.cpp
)

mdt_add_library(
  NAME Expected
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core
)
mdt_set_library_description(
  NAME Expected
  DESCRIPTION "Expected provides a basic support of expected value."
)
