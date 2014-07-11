# Project file for Multi Diag Tools
# * Part: mdtutilsgui *
# * Directory: mdutilsgui *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # mdtLed
          mdtLed.h
          mdtBlinkLed.h
          mdtDoubleEdit.h
          mdtDoubleValidator.h
          # I/O Widgets
          mdtAbstractIoWidget.h
          mdtAnalogInWidget.h
          mdtAnalogOutWidget.h
          mdtDigitalInWidget.h
          mdtDigitalOutWidget.h
          # Database
          mdtSqlTableModel.h
          #mdtSqlDataWidgetMapper.h
          mdtDataTableItemDelegate.h
          #mdtDataTableManager.h
          mdtDataTableModel.h
          mdtFieldMapDialog.h
          mdtFieldMapItemDialog.h
          mdtFieldListEditionDialog.h
          mdtSqlQueryWidget.h
          mdtItemsSelectorDialog.h
          mdtMvColumnSelectionDialog.h
          mdtMvSortingSetupDialog.h
          mdtSqlRelation.h
          mdtSqlFieldHandler.h
          mdtAbstractSqlWidget.h
          mdtSqlFormWidget.h
          #mdtSqlTableWidget.h
          #mdtSqlSelectionDialog.h
          mdtSortFilterProxyModel.h
          mdtSqlDataValidator.h
          mdtSqlFormWidgetDataValidator.h
          #mdtSqlDatabaseManager.h
          # Essais
          mdtParentChildTableModel.h
          mdtParentChildTableItem.h
    )

# Liste des fichiers sources
set (SOURCES_FILES 
          # mdtLed
          mdtLed.cpp
          mdtBlinkLed.cpp
          mdtDoubleEdit.cpp
          mdtDoubleValidator.cpp
          # I/O Widgets
          mdtAbstractIoWidget.cpp
          mdtAnalogInWidget.cpp
          mdtAnalogOutWidget.cpp
          mdtDigitalInWidget.cpp
          mdtDigitalOutWidget.cpp
          # Database
          mdtSqlTableModel.cpp
          #mdtSqlDataWidgetMapper.cpp
          mdtDataTableItemDelegate.cpp
          #mdtDataTableManager.cpp
          mdtDataTableModel.cpp
          mdtFieldMap.cpp
          mdtFieldMapField.cpp
          mdtFieldMapItem.cpp
          mdtFieldMapDialog.cpp
          mdtFieldMapItemDialog.cpp
          mdtFieldListEditionDialog.cpp
          mdtSqlQueryWidget.cpp
          mdtItemsSelectorDialog.cpp
          mdtItemsSelectorDialogItem.cpp
          mdtMvColumnSelectionDialog.cpp
          mdtMvSortingSetupDialog.cpp
          mdtSqlRelation.cpp
          mdtSqlRelationItem.cpp
          mdtSqlFieldHandler.cpp
          mdtAbstractSqlWidget.cpp
          mdtSqlFormWidget.cpp
          #mdtSqlTableWidget.cpp
          #mdtSqlSelectionDialog.cpp
          mdtSortFilterProxyModel.cpp
          mdtSqlDataValidator.cpp
          mdtSqlFormWidgetDataValidator.cpp
          #mdtSqlSchemaTable.cpp
          #mdtSqlDatabaseManager.cpp
          # Essais
          mdtParentChildTableModel.cpp
          mdtParentChildTableItem.cpp
    )

# Qt ui files
set (FORM_FILES
          mdtSqlQueryWidget.ui
          mdtSqlWindow.ui
          mdtItemsSelectorDialog.ui
          mdtFieldMapDialog.ui
          mdtFieldMapItemDialog.ui
          mdtFieldListEditionDialog.ui
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
add_library (mdtutilsgui${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})
target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
if(WIN32)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "qwt")
  else(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "qwtd")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
else(WIN32)
  target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "qwt")
endif(WIN32)


# Version statique
# if(MDT_BUILD_STATIC_LIBS)
#   add_library (mdtutilsgui_static STATIC ${SOURCES_FILES} )
#   target_link_libraries(mdtutilsgui_static ${QT_LIBRARIES})
#   set_target_properties("mdtutilsgui_static" PROPERTIES OUTPUT_NAME "mdtutils")
# endif(MDT_BUILD_STATIC_LIBS)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtutilsgui" COMPONENT dev)

# Destinations
install(TARGETS "mdtutilsgui${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#if(MDT_BUILD_STATIC_LIBS)
#  install(TARGETS "mdtutilsgui_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#endif(MDT_BUILD_STATIC_LIBS)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtutilsgui${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so" "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg" "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
