
find_package(Qt5 COMPONENTS Network)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  # qtlocalpeer.cpp includes qtlockedfile.cpp and qtlockedfile_<platform>.cpp in its source
  qtlocalpeer.cpp 
  qtsinglecoreapplication.cpp
)

mdt_add_library(
  NAME QtSingleApplication_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Network
)
target_compile_definitions(QtSingleApplication_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)
if(WIN32)
 target_compile_definitions(QtSingleApplication_Core PRIVATE UNICODE _UNICODE)
endif()

mdt_set_library_description(
  NAME QtSingleApplication_Core
  DESCRIPTION "QtSingleCoreApplication helps to run a application only once if needed."
)
