
set(SOURCE_FILES
  Mdt/Numeric/Double.cpp
  Mdt/Numeric/Resistance.cpp
  Mdt/Numeric/Length.cpp
  Mdt/Numeric/IntegralRange.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME Numeric
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core
)
mdt_set_library_description(
  NAME Numeric
  DESCRIPTION "Numeric provides common stuff for numerics."
)
