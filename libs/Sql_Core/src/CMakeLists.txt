
# List of source files
set(SOURCE_FILES
    Mdt/Sql/LibraryInfo.cpp
    Mdt/Sql/PluginInfo.cpp
    Mdt/Sql/PluginLoader.cpp
    Mdt/Sql/PluginInstance.cpp
    Mdt/Sql/AbstractErrorDriver.cpp
    Mdt/Sql/AbstractErrorDriverPlugin.cpp
    Mdt/Sql/ErrorDriver.cpp
    Mdt/Sql/Error.cpp
    Mdt/Sql/FieldNameValueMap.cpp
    Mdt/Sql/PrimaryKeyRecord.cpp
#     Mdt/Sql/Query.cpp
    Mdt/Sql/AbstractQuery.cpp
    Mdt/Sql/InsertQuery.cpp
    Mdt/Sql/UpdateStatement.cpp
    Mdt/Sql/UpdateQuery.cpp
    Mdt/Sql/DeleteStatement.cpp
    Mdt/Sql/DeleteQuery.cpp
    Mdt/Sql/SelectTable.cpp
    Mdt/Sql/SelectFieldSqlTransform.cpp
    Mdt/Sql/SelectFieldItem.cpp
    Mdt/Sql/SelectFieldItemSqlTransform.cpp
    Mdt/Sql/SelectFieldListSqlTransform.cpp
    Mdt/Sql/Expression/TerminalSqlTransform.cpp
    Mdt/Sql/Expression/ComparisonSqlTransform.cpp
    Mdt/Sql/Expression/LogicalSqlTransform.cpp
    Mdt/Sql/Expression/LikeExpressionSqlTransform.cpp
    Mdt/Sql/JoinConstraintFieldPairList.cpp
    Mdt/Sql/JoinConstraintFieldPairListSqlTransform.cpp
    Mdt/Sql/JoinConstraintExpression.cpp
    Mdt/Sql/JoinConstraint.cpp
    Mdt/Sql/JoinConstraintSqlTransform.cpp
    Mdt/Sql/JoinClauseItemSqlTransform.cpp
    Mdt/Sql/JoinClauseItemListSqlTransform.cpp
    Mdt/Sql/JoinClause.cpp
    Mdt/Sql/JoinClauseSqlTransform.cpp
    Mdt/Sql/FromClause.cpp
    Mdt/Sql/FromClauseSqlTransform.cpp
    Mdt/Sql/WhereExpression.cpp
    Mdt/Sql/SelectStatement.cpp
    Mdt/Sql/SelectStatementSqlTransform.cpp
    Mdt/Sql/ConnectionParameters.cpp
    Mdt/Sql/AbstractSelectQuery.cpp
    Mdt/Sql/SelectQuery.cpp
    Mdt/Sql/AsyncQuery.cpp
    Mdt/Sql/AsyncQueryConnection.cpp
    Mdt/Sql/AsyncQueryThreadWorker.cpp
    Mdt/Sql/Schema/Driver.cpp
    Mdt/Sql/Schema/DriverImplementationInterface.cpp
    Mdt/Sql/Schema/DriverSQLite.cpp
    Mdt/Sql/Schema/DriverMySql.cpp
    Mdt/Sql/Schema/DriverPostgreSQL.cpp
    Mdt/Sql/Schema/FieldLength.cpp
    Mdt/Sql/Schema/FieldTypeFlags.cpp
    Mdt/Sql/Schema/FieldList.cpp
    Mdt/Sql/Schema/FieldIndexList.cpp
    Mdt/Sql/Schema/FieldTypeMapImplementation.cpp
    Mdt/Sql/Schema/FieldTypeMap.cpp
    Mdt/Sql/Schema/FieldTypeName.cpp
    Mdt/Sql/Schema/Field.cpp
#     Mdt/Sql/Schema/FieldTypeListModel.cpp
    Mdt/Sql/Schema/PrimaryKeyFieldIndexList.cpp
    Mdt/Sql/Schema/PrimaryKeyType.cpp
    Mdt/Sql/Schema/PrimaryKeyContainer.cpp
    Mdt/Sql/Schema/ForeignKeyActionSqlTransform.cpp
    Mdt/Sql/Schema/ForeignKey.cpp
    Mdt/Sql/Schema/ForeignKeyList.cpp
    Mdt/Sql/Schema/Index.cpp
#     Mdt/Sql/Schema/IndexListModel.cpp
    Mdt/Sql/Schema/Table.cpp
#     Mdt/Sql/Schema/TableModel.cpp
    Mdt/Sql/Schema/View.cpp
    Mdt/Sql/Schema/ViewSqlTransform.cpp
    Mdt/Sql/Schema/Schema.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_library(
  NAME Sql_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Expected LibraryInfo_Core Error_Core Algorithm Container Qt5::Sql FilterExpression # TODO remove FilterExpression once possible
)
mdt_set_library_description(
  NAME Sql_Core
  DESCRIPTION "Sql provides additions for database (schema, ...)."
)
# target_compile_definitions(Sql_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
