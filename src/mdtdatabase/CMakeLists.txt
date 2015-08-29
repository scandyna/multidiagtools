# Project file for Multi Diag Tools
# * Part: mdtdatabase *
# * Directory: mdtdatabase *

# Project
project (multidiagtools)

# Liste des fichiers sources
set (SOURCES_FILES 
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
          mdtDataTableManager.cpp
          mdtSqlDatabaseManager.cpp
          
          mdtSqlConnectionNameWidget.cpp
          mdtSqlDatabaseSqlite.cpp
          mdtSqlDatabaseDialogSqlite.cpp
          mdtSqlSchemaTable.cpp
          mdtSqlSchemaTableModel.cpp
          mdtSqlFieldSelectionDialog.cpp
          mdtSqlTransaction.cpp
          mdtSqlForeignKeySetting.cpp
          mdtSqlFieldSetupWidget.cpp
          mdtSqlFieldMappingDialog.cpp
          mdtSqlTableMappingWidget.cpp
          mdtSqlTableMappingWidgetItem.cpp
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
