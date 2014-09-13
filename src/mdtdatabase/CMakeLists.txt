# Project file for Multi Diag Tools
# * Part: mdtdatabase *
# * Directory: mdtdatabase *

# Project
project (multidiagtools)


# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtAbstractSqlTableController.h
          mdtSqlFieldHandler.h
          mdtSqlDataValidator.h
          mdtSqlFormWidgetDataValidator.h
          mdtSqlDataWidgetController.h
          mdtSqlTableViewController.h
          mdtSqlForm.h
          mdtSqlWindow.h
          mdtSqlDialog.h
          mdtSqlSelectionDialog.h
          mdtSqlFormWidget.h
          mdtSqlTableWidget.h
          mdtDataTableManager.h
          mdtSqlDatabaseManager.h
    )

# Liste des fichiers sources
set (SOURCES_FILES 
          mdtAbstractSqlTableController.cpp
          mdtSqlFieldHandler.cpp
          mdtSqlDataValidator.cpp
          mdtSqlRelationInfo.cpp
          mdtSqlFormWidgetDataValidator.cpp
          mdtSqlDataWidgetController.cpp
          mdtSqlTableViewController.cpp
          mdtSqlForm.cpp
          mdtSqlWindow.cpp
          mdtSqlDialog.cpp
          mdtSqlRecord.cpp
          mdtSqlSelectionDialog.cpp
          mdtSqlFormWidget.cpp
          mdtSqlTableWidget.cpp
          mdtSqlTableSelectionItem.cpp
          mdtSqlTableSelectionRow.cpp
          mdtSqlTableSelection.cpp
          mdtDataTableManager.cpp
          mdtSqlDatabaseManager.cpp
          mdtSqlSchemaTable.cpp
    )

# Qt ui files
set (FORM_FILES
          mdtSqlWindow.ui
          mdtSqlSelectionDialog.ui
    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  set (SOURCES_FILES ${SOURCES_FILES} 
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(UNIX)

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtdatabase${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtdatabase${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})
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
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so" "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg" "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libmdtdatabase${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
