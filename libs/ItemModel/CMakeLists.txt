# Project file for Multi Diag Tools

# Project
project(multidiagtools)

# List of source files
set(SOURCES_FILES
    src/Mdt/ItemModel/Range.cpp
    src/Mdt/ItemModel/RowRange.cpp
    src/Mdt/ItemModel/ColumnRange.cpp
    src/Mdt/ItemModel/IndexFormatMapItem.cpp
    src/Mdt/ItemModel/IndexFormatMap.cpp
    src/Mdt/ItemModel/RowOrColumnFormatMapItem.cpp
    src/Mdt/ItemModel/RowColumnFormatMapBase.cpp
    src/Mdt/ItemModel/RowFormatMap.cpp
    src/Mdt/ItemModel/ColumnFormatMap.cpp
    src/Mdt/ItemModel/FormatMap.cpp
    src/Mdt/ItemModel/VariantTableModel.cpp
    src/Mdt/ItemModel/VariantTableModelItem.cpp
    src/Mdt/ItemModel/SortFilterProxyModel.cpp
    src/Mdt/ItemModel/Expression/FilterExpressionContainer.cpp
    src/Mdt/ItemModel/Expression/ComparisonEval.cpp
    src/Mdt/ItemModel/Expression/GetRelationKeyForEquality.cpp
    src/Mdt/ItemModel/Expression/GreatestColumnTransform.cpp
    src/Mdt/ItemModel/FilterExpression.cpp
    src/Mdt/ItemModel/FilterProxyModel.cpp
    src/Mdt/ItemModel/RelationFilterExpression.cpp
    src/Mdt/ItemModel/RelationFilterProxyModel.cpp
    src/Mdt/ItemModel/ColumnSortStringAttributesList.cpp
    src/Mdt/ItemModel/SortProxyModel.cpp
    src/Mdt/ItemModel/FormatProxyModel.cpp
    src/Mdt/ItemModel/ProxyModelContainer.cpp
    src/Mdt/ItemModel/RowColumnListBase.cpp
    src/Mdt/ItemModel/RowList.cpp
    src/Mdt/ItemModel/ColumnList.cpp
    src/Mdt/ItemModel/RelationKey.cpp
    src/Mdt/ItemModel/RelationKeyCopier.cpp
)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif()

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Error/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Algorithm/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../FilterExpression/src")
# Because some headers are generated during building (config.h, Qt's UIC), we also include build path
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library name
set(LIBRARY_NAME "ItemModel")

# Output library technical names
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LC)
set(LIBRARY_BIN_NAME "mdt${LIBRARY_NAME_LC}")
# Add output library
add_library(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterror${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdtalgorithm${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdtfilterexpression${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} Qt5::Core)
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} Qt5::Gui)

# Translation files
# # set(TS_FILES
# #     ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_fr.ts
# #     ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_de.ts
# # )
# # qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
# # add_custom_target("Mdt${LIBRARY_NAME}Ts" DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/Mdt/${LIBRARY_NAME}" COMPONENT dev)

# Install targets
install(TARGETS "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  mdtSeparateDebugSymbols(${LIBRARY_BIN_NAME})
endif(UNIX)

# Unit tests
add_subdirectory("tests")
