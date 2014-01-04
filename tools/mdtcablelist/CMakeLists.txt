# CMake project file for Multi Diag Tools
# Tool: mdtcablelist

# Project
project (multidiagtools)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Header files containing Qt Q_OBJECT macro
set (HEADER_FILES
          src/mdtClEditor.h
          src/mdtClVehicleTypeEditor.h
          src/mdtClConnectorEditor.h
          src/mdtClArticleEditor.h
          src/mdtClArticleConnectionDialog.h
          src/mdtClUnitEditor.h
          src/mdtClLinkEditor.h
          src/mdtClUnitConnectionDialog.h
          src/mdtClUnitConnectionSelectionDialog.h
          src/mdtClUnitLinkDialog.h
          src/mdtClTerminalBlockEditor.h
          src/mdtClTerminalEditorDialog.h
          src/mdtClArticleComponentDialog.h
          src/mdtClArticleLinkDialog.h
          src/mdtClPathGraphDialog.h
          src/mdtClMainWindow.h
          src/mdtCcTestConnectionCableEditor.h
          src/mdtTtTestNodeEditor.h
          src/mdtTtTestModelEditor.h
          src/mdtTtTestModelItemEditor.h
          src/mdtTtCableChecker.h
          src/mdtTtCableCheckerWindow.h
    )

# Source files
set (SOURCES_FILES 
          # Main
          src/main.cpp
          src/mdtClBase.cpp
          src/mdtClEditor.cpp
          src/mdtClVehicleTypeEditor.cpp
          src/mdtClConnectorEditor.cpp
          src/mdtClConnectionData.cpp
          src/mdtClArticleConnectionData.cpp
          src/mdtClArticleEditor.cpp
          src/mdtClArticleConnectionDialog.cpp
          src/mdtClUnit.cpp
          src/mdtClUnitEditor.cpp
          src/mdtClUnitConnectionData.cpp
          src/mdtClLinkEditor.cpp
          src/mdtClUnitConnectionDialog.cpp
          src/mdtClUnitConnectionSelectionDialog.cpp
          src/mdtClUnitLinkDialog.cpp
          src/mdtClLinkData.cpp
          src/mdtClTerminalBlockEditor.cpp
          src/mdtClTerminalEditorDialog.cpp
          src/mdtClUnitVehicleType.cpp
          src/mdtClArticleComponentDialog.cpp
          src/mdtClArticleLinkDialog.cpp
          src/mdtClArticle.cpp
          src/mdtClPathGraphicsConnection.cpp
          src/mdtClPathGraphicsLink.cpp
          src/mdtClPathGraph.cpp
          src/mdtClPathGraphDialog.cpp
          src/mdtClDatabaseSchema.cpp
          src/mdtClMainWindow.cpp
          src/mdtCcTestConnectionCable.cpp
          src/mdtCcTestConnectionCableEditor.cpp
          src/mdtTtTestNodeEditor.cpp
          src/mdtTtTestNode.cpp
          src/mdtTtTestModelEditor.cpp
          src/mdtTtTestModel.cpp
          src/mdtTtTestModelItemEditor.cpp
          src/mdtTtTestModelItem.cpp
          src/mdtTtTest.cpp
          src/mdtTtCableChecker.cpp
          src/mdtTtCableCheckerWindow.cpp
    )

# Qt ui files
set (FORM_FILES
          src/mdtClVehicleTypeEditor.ui
          src/mdtClConnectorEditor.ui
          src/mdtClArticleEditor.ui
          src/mdtClArticleConnectionDialog.ui
          src/mdtClUnitEditor.ui
          src/mdtClUnitConnectionDialog.ui
          src/mdtClUnitLinkDialog.ui
#          src/mdtClUnitConnectionEditor.ui
          src/mdtClLinkEditor.ui
          src/mdtClTerminalBlockEditor.ui
          src/mdtClArticleComponentDialog.ui
          src/mdtClArticleLinkDialog.ui
          src/mdtClPathGraphDialog.ui
          src/mdtClMainWindow.ui
          src/mdtTtTestNodeEditor.ui
          src/mdtTtTestModelEditor.ui
          src/mdtTtTestModelItemEditor.ui
          src/mdtCcTestConnectionCableEditor.ui
          src/mdtTtCableChecker.ui
          src/mdtTtCableCheckerWindow.ui
    )

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Executable
add_executable (mdtcablelist${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraries to link
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} "mdtdport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})

# Install targets
install (TARGETS "mdtcablelist${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)

#################################################################################################################################################
# *** Unit tests section ***
#################################################################################################################################################

# ** Unit tests compilation **
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
