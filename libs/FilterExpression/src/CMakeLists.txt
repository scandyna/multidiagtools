
set(SOURCE_FILES
    Mdt/FilterExpression/LikeExpressionRegexTransform.cpp
)

find_package(Qt5 COMPONENTS Core)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
#  if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#  endif(Boost_FOUND)
endif()

mdt_add_library(
  NAME FilterExpression
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Algorithm Qt5::Core
)
target_include_directories(FilterExpression PUBLIC ${Boost_INCLUDE_DIRS})
mdt_set_library_description(
  NAME FilterExpression
  DESCRIPTION "FilterExpression provides common stuff for filter EDSL."
)
