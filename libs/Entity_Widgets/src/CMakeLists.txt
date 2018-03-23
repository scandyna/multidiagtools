
# List of source files
set(SOURCE_FILES
  Mdt/Entity/SetupWidget.cpp
  Mdt/Entity/FieldAttributesItemDelegate.cpp
)

find_package(Qt5 COMPONENTS Widgets)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
# if(NOT Boost_FOUND)
#   find_package(Boost REQUIRED)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#   endif(Boost_FOUND)
# endif()

mdt_add_library(
  NAME Entity_Widgets
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Entity_Core Qt5::Widgets;
)
target_compile_definitions(Entity_Widgets PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Entity_Widgets
  DESCRIPTION "Possible introspection of a entity."
)
