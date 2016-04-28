# Project file for Multi Diag Tools
# * Part: mdtutilsgui *
# * Directory: mdutilsgui *

# Project
project (multidiagtools)

# Liste des fichiers sources
set (SOURCES_FILES 
          # mdtLed
          mdtLed.cpp
          mdtBlinkLed.cpp
          mdtDoubleEdit.cpp
          mdtDoubleEditDialog.cpp
          mdtDoubleValidator.cpp
          # I/O Widgets
          mdtAbstractIoWidget.cpp
          mdtAnalogInWidget.cpp
          mdtAnalogOutWidget.cpp
          mdtDigitalInWidget.cpp
          mdtDigitalOutWidget.cpp
          # Database
          mdtDataTableItemDelegate.cpp
          mdtSqlQueryWidget.cpp
          mdtItemsSelectorDialog.cpp
          mdtItemsSelectorDialogItem.cpp
          mdtMvColumnSelectionDialog.cpp
          mdtMvSortingSetupDialog.cpp
          # Item model
          mdtSortFilterProxyModel.cpp
          mdtFormatProxyModel.cpp
          mdtFormatProxyModelSettings.cpp
          mdtComboBoxItemDelegate.cpp
          mdtProgressBarItemDelegate.cpp
          # Essais
          mdtParentChildTableModel.cpp
          mdtParentChildTableItem.cpp
          mdtUiMessageHandler.cpp
          mdtQActionEnableStateGuard.cpp
    )

# Librairie du projet
add_library (mdtutilsgui${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} Qt5::Widgets Qt5::Sql)
target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "mdterrorgui${MDTLIB_VERSION_SUFFIX}")
if(WIN32)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "qwt")
  else(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} "qwtd")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
else(WIN32)
  target_link_libraries(mdtutilsgui${MDTLIB_VERSION_SUFFIX} ${QWT_LIBRARY})
endif(WIN32)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtutilsgui" COMPONENT dev)

# Destinations
install(TARGETS "mdtutilsgui${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtutilsgui${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so" "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg" "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libmdtutilsgui${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
