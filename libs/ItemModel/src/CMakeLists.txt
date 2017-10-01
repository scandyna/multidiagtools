
# List of source files
set(SOURCE_FILES
    Mdt/ItemModel/Range.cpp
    Mdt/ItemModel/RowRange.cpp
    Mdt/ItemModel/ColumnRange.cpp
    Mdt/ItemModel/IndexFormatMapItem.cpp
    Mdt/ItemModel/IndexFormatMap.cpp
    Mdt/ItemModel/RowOrColumnFormatMapItem.cpp
    Mdt/ItemModel/RowColumnFormatMapBase.cpp
    Mdt/ItemModel/RowFormatMap.cpp
    Mdt/ItemModel/ColumnFormatMap.cpp
    Mdt/ItemModel/FormatMap.cpp
    Mdt/ItemModel/VariantTableModel.cpp
    Mdt/ItemModel/VariantTableModelItem.cpp
    Mdt/ItemModel/SortFilterProxyModel.cpp
    Mdt/ItemModel/Expression/FilterExpressionContainer.cpp
    Mdt/ItemModel/Expression/ComparisonEval.cpp
    Mdt/ItemModel/Expression/GetRelationKeyForEquality.cpp
    Mdt/ItemModel/Expression/GreatestColumnTransform.cpp
    Mdt/ItemModel/FilterExpression.cpp
    Mdt/ItemModel/FilterProxyModel.cpp
    Mdt/ItemModel/RelationFilterExpression.cpp
    Mdt/ItemModel/RelationFilterProxyModel.cpp
    Mdt/ItemModel/ColumnSortStringAttributesList.cpp
    Mdt/ItemModel/SortProxyModel.cpp
    Mdt/ItemModel/FormatProxyModel.cpp
    Mdt/ItemModel/ProxyModelContainer.cpp
    Mdt/ItemModel/RowColumnListBase.cpp
    Mdt/ItemModel/RowList.cpp
    Mdt/ItemModel/ColumnList.cpp
    Mdt/ItemModel/ForeignKeyList.cpp
    Mdt/ItemModel/KeyData.cpp
    Mdt/ItemModel/KeyRecord.cpp
    Mdt/ItemModel/PrimaryKeyRecord.cpp
    Mdt/ItemModel/ForeignKeyRecord.cpp
    Mdt/ItemModel/RelationKey.cpp
    Mdt/ItemModel/RelationKeyCopier.cpp
    Mdt/ItemModel/PkFkProxyModelBase.cpp
    Mdt/ItemModel/PrimaryKeyProxyModel.cpp
    Mdt/ItemModel/ForeignKeyProxyModelMapItem.cpp
    Mdt/ItemModel/ForeignKeyProxyModelMap.cpp
    Mdt/ItemModel/ForeignKeyProxyModel.cpp
    Mdt/ItemModel/HeaderProxyModelItem.cpp
    Mdt/ItemModel/HeaderProxyModelItemList.cpp
    Mdt/ItemModel/HeaderProxyModel.cpp
)

find_package(Qt5 COMPONENTS Core Gui)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif()

mdt_add_library(
  NAME ItemModel
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Algorithm FilterExpression Qt5::Core Qt5::Gui
)
mdt_set_library_description(
  NAME ItemModel
  DESCRIPTION "ItemModel provides additions for Qt item/view models."
)
