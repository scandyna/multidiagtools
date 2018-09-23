
# List of source files
set(SOURCE_FILES 
   Mdt/LibraryInfo.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME LibraryInfo_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Expected Qt5::Core
)
if(UNIX)
  target_sources(LibraryInfo_Core PRIVATE Mdt/LibraryInfoImpl_Unix.cpp)
endif()
if(WIN32)
  target_sources(LibraryInfo_Core PRIVATE Mdt/LibraryInfoImpl_Windows.cpp)
endif()

target_compile_definitions(LibraryInfo_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME LibraryInfo_Core
  DESCRIPTION "Provides helpers to get informations about a Mdt library."
)

