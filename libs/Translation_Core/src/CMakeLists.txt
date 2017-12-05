
# List of source files
set(SOURCE_FILES
  Mdt/Translation/LanguageCode.cpp
  Mdt/Translation/LanguageCodeList.cpp
  Mdt/Translation/QmFileName.cpp
  Mdt/Translation/TranslationInfo.cpp
  Mdt/Translation/TranslationInfoList.cpp
)

find_package(Qt5 COMPONENTS Core)

# # Get Boost include directory if not allready set
# # We only use header onyl libraries, so we not use COMPONENTS
# # See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
# if(NOT Boost_FOUND)
#   find_package(Boost REQUIRED)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#   endif(Boost_FOUND)
# endif()

mdt_add_library(
  NAME Translation_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core
)
target_compile_definitions(Translation_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Translation_Core
  DESCRIPTION "Translation provides some helper for translations (like finding QM files)."
)
