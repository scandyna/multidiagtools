# Project file for Multi Diag Tools
# * Part: mdtdatabase *
# * Directory: mdtdatabase *

# Project
project (multidiagtools)

# Liste des fichiers sources
set (SOURCES_FILES
          mdt/sql/error/Error.cpp
          mdt/sql/Database.cpp
          mdtAbstractSqlTableController.cpp
          mdtSqlFieldHandler.cpp
          mdtSqlDataValidator.cpp
          mdtSqlRelation.cpp
          mdtSqlRelationItem.cpp
          mdtSqlRelationInfo.cpp
          mdtSqlFormWidgetDataValidator.cpp
          mdtSqlDataWidgetController.cpp
          mdtSqlTableViewController.cpp
          mdtSqlForm.cpp
          mdtSqlWindow.cpp
          mdtSqlDialog.cpp
          mdtSqlRecord.cpp
          mdtSqlSelectionDialog.cpp
          mdtSqlTableWidget.cpp
          mdtSqlTableSelectionItem.cpp
          mdtSqlTableSelectionRow.cpp
          mdtSqlTableSelection.cpp
          mdtSqlConnectionNameWidget.cpp
          mdtSqlDatabaseBasicInfoWidget.cpp
          mdtSqlDatabaseSqlite.cpp
          mdtSqlDatabaseDialogSqlite.cpp
          # Field selection
          mdt/sql/FieldSelectionProxyModel.cpp
          mdt/sql/FieldSelectionDialog.cpp
          mdtSqlTransaction.cpp
          mdtSqlForeignKeySetting.cpp
          # Database schema
          mdtSqlDatabaseSchema.cpp
          mdtSqlDatabaseSchemaModel.cpp
          mdtSqlDatabaseSchemaDialog.cpp
          mdtSqlDatabaseSchemaThread.cpp
          mdtSqlTableSchema.cpp
          mdtSqlViewSchema.cpp
          mdtSqlTriggerSchema.cpp
          mdtSqlTableSchemaModel.cpp
          mdtSqlTableSetupWidget.cpp
          mdtSqlIndex.cpp
          mdtSqlPrimaryKey.cpp
          mdtSqlForeignKey.cpp
          mdtSqlField.cpp
          mdtSqlFieldType.cpp
          mdtSqlFieldSetupWidget.cpp
          mdtSqlFieldSetupDialog.cpp
          # Copier common classes
          mdt/sql/copier/FieldTypeMapCheck.cpp
          mdt/sql/copier/TableMapping.cpp
          mdt/sql/copier/TableMappingItem.cpp
          mdt/sql/copier/TableMappingModel.cpp
          mdt/sql/copier/TableMappingDialog.cpp
          mdt/sql/copier/MappingModel.cpp
          mdt/sql/copier/CopyThread.cpp
          mdt/sql/copier/ExpressionKeyFieldSelectionDialog.cpp
          mdt/sql/copier/ExpressionMatchItemModel.cpp
          mdt/sql/copier/RelatedTableInsertMatchItemModel.cpp
          mdt/sql/copier/RelatedTableInsertExpressionDialog.cpp
          mdt/sql/copier/UniqueInsertCriteriaDialog.cpp
          mdt/sql/copier/CopyHelper.cpp
          # Database copier classes
          mdt/sql/copier/DatabaseCopierTableMapping.cpp
          mdt/sql/copier/DatabaseCopierTableMappingModel.cpp
          mdt/sql/copier/DatabaseCopierTableMappingDialog.cpp
          mdt/sql/copier/DatabaseMapping.cpp
          mdt/sql/copier/DatabaseMappingModel.cpp
          mdt/sql/copier/DatabaseCopyThread.cpp
          mdt/sql/copier/DatabaseCopyDialog.cpp
          # CSV import classes
          mdt/sql/copier/CsvImportTableMapping.cpp
          mdt/sql/copier/CsvStringImportTableMapping.cpp
          mdt/sql/copier/CsvStringImportTableMappingModel.cpp
          mdt/sql/copier/CsvFileImportTableMapping.cpp
          mdt/sql/copier/CsvFileImportTableMappingModel.cpp
          mdt/sql/copier/CsvFileImportTableMappingDialog.cpp
          mdt/sql/copier/CsvFileImportMapping.cpp
          mdt/sql/copier/CsvFileImportMappingModel.cpp
          mdt/sql/copier/CsvFileImportDialog.cpp
          mdt/sql/copier/CsvFileImportThread.cpp
    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  set (SOURCES_FILES ${SOURCES_FILES} 
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(UNIX)

# Librairie du projet
add_library (mdtdatabase${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(mdtdatabase${MDTLIB_VERSION_SUFFIX} Qt5::Widgets Qt5::Sql)
target_link_libraries(mdtdatabase${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabase${MDTLIB_VERSION_SUFFIX} "mdtcsv${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabase${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtdatabase" COMPONENT dev)

# Destinations
install(TARGETS "mdtdatabase${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtdatabase${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so" "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg" "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
