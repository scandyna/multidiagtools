# Project file for Multi Diag Tools
# * Part: mdttesttool *
# * Directory: mdttools/mdttesttool *

# Project
project (multidiagtools)

# Header files for mdtTestTool
set (TT_HEADER_FILES 
          mdtTtTestSystemEditor.h
#           mdtTtTestSystemComponent.h
          mdtTtTestSystemComponentEditor.h
          mdtTtTestSystemUnitEditor.h

          mdtTtApplicationWidgets.h
          mdtTtTestCableEditor.h
          mdtTtLogicalTestCableEditor.h
          mdtTtLogicalTestCableDialog.h
          mdtTtLogicalTestCableDutWidget.h
          mdtTtLogicalTestCableTsWidget.h
          mdtTtTestCableOffsetTool.h
          mdtTtTestLinkDialog.h
          mdtTtTestNodeEditor.h
          mdtTtTestNodeUnitDialog.h
          mdtTtTestNodeRouteDialog.h
          mdtTtTestModelEditor.h
          mdtTtTestModelItemEditor.h
          mdtTtTestNodeUnitSetupDialog.h
          mdtTtTestNodeManager.h
          mdtTtTestNodeModbusIoTool.h
          #mdtTtTestNodeManagerWidget.h
          mdtTtBasicTesterWindow.h
          mdtTtTest.h
          mdtTtTestModelGenerationDialog.h
          mdtTtAbstractTester.h
          mdtTtAbstractTestNodeCalibrationTool.h
          mdtTtBasicTester.h
          mdtTtBasicTestNodeCalibrationTool.h
          mdtTtBasicTestNodeCalibrationWindow.h
          mdtTtValueEditionDialog.h
          mdtTtTestStep.h
          mdtTtTestStepContainer.h
          mdtTtTestStepWidget.h
          mdtTtTestStepContainerWidget.h
#           mdtTtValueLimits.h
          mdtTtValueLimitsWidget.h
    )

# Sources files for mdtTestTool
set (TT_SOURCES_FILES 
          mdtTtTestSystemEditor.cpp
          mdtTtTestSystemComponent.cpp
          mdtTtTestSystemComponentEditor.cpp
          mdtTtTestSystemUnitEditor.cpp

          mdtTtApplicationWidgets.cpp
          mdtTtBase.cpp
          mdtTtDatabaseSchema.cpp
          mdtTtTestLinkData.cpp
          mdtTtTestLinkDialog.cpp
          mdtTtTestCableEditor.cpp
          mdtTtLogicalTestCable.cpp
          mdtTtLogicalTestCableEditor.cpp
          mdtTtLogicalTestCableDialog.cpp
          mdtTtLogicalTestCableDutWidget.cpp
          mdtTtLogicalTestCableTsWidget.cpp
          mdtTtTestCableOffsetTool.cpp
          mdtTtTestNode.cpp
          mdtTtTestNodeEditor.cpp
          mdtTtTestNodeUnit.cpp
          mdtTtTestNodeUnitData.cpp
          mdtTtTestNodeUnitDialog.cpp
          mdtTtTestNodeRouteData.cpp
          mdtTtTestNodeRoute.cpp
          mdtTtTestNodeRouteDialog.cpp
          mdtTtTestModelEditor.cpp
          mdtTtTestModel.cpp
          mdtTtTestModelItem.cpp
          mdtTtTestModelItemData.cpp
          mdtTtTestModelItemEditor.cpp
          mdtTtTestNodeUnitSetupData.cpp
          mdtTtTestNodeUnitSetupDialog.cpp
          mdtTtTestModelItemRouteData.cpp
          mdtTtTestItemNodeSetupData.cpp
          mdtTtTestNodeSetupData.cpp
          mdtTtTestNodeManager.cpp
          mdtTtTestNodeModbusIoTool.cpp
          #mdtTtTestNodeManagerWidget.cpp
          mdtTtBasicTesterWindow.cpp
          mdtTtTestModelData.cpp
          mdtTtTestData.cpp
          mdtTtTest.cpp
          mdtTtTestModelGenerationDialog.cpp
          mdtTtAbstractTester.cpp
          mdtTtAbstractTestNodeCalibrationTool.cpp
          mdtTtBasicTester.cpp
          mdtTtBasicTestNodeCalibrationTool.cpp
          mdtTtBasicTestNodeCalibrationWindow.cpp
          mdtTtValueEditionDialog.cpp
          mdtTtTestStep.cpp
          mdtTtTestStepContainer.cpp
          mdtTtTestStepWidget.cpp
          mdtTtTestStepContainerWidget.cpp
          mdtTtValueLimits.cpp
          mdtTtValueLimitsWidget.cpp
    )

# Form files for mdtTestTool
set (TT_FORM_FILES
          mdtTtTestSystemEditor.ui
          mdtTtTestSystemComponentEditor.ui
          mdtTtTestSystemUnitEditor.ui

          mdtTtTestCableEditor.ui
          mdtTtLogicalTestCableEditor.ui
          mdtTtLogicalTestCableDialog.ui
          mdtTtLogicalTestCableDutWidget.ui
          mdtTtLogicalTestCableTsWidget.ui
          mdtTtTestLinkDialog.ui
          mdtTtTestNodeEditor.ui
          mdtTtTestNodeUnitDialog.ui
          mdtTtTestModelEditor.ui
          mdtTtTestModelItemEditor.ui
          mdtTtTestNodeUnitSetupDialog.ui
          mdtTtBasicTesterWindow.ui
          mdtTtBasicTestNodeCalibrationWindow.ui
          mdtTtTestNodeRouteDialog.ui
          mdtTtTestModelGenerationDialog.ui
          mdtTtValueEditionDialog.ui
          mdtTtValueLimitsWidget.ui
    )

# Header files for mdtCableList
set (CL_HEADER_FILES 
          mdtcablelist/mdtClApplicationWidgets.h
          mdtcablelist/mdtClVehicleTypeEditor.h
          mdtcablelist/mdtClConnectorEditor.h
          mdtcablelist/mdtClConnectorSelectionDialog.h
          mdtcablelist/mdtClConnectorContactSelectionDialog.h
          mdtcablelist/mdtClArticleEditor.h
          mdtcablelist/mdtClArticleComponentDialog.h
          mdtcablelist/mdtClArticleConnectionDialog.h
          mdtcablelist/mdtClArticleLinkDialog.h
          mdtcablelist/mdtClArticleSelectionDialog.h
          mdtcablelist/mdtClPathGraphDialog.h
          mdtcablelist/mdtClUnitConnectionDialog.h
          mdtcablelist/mdtClUnitEditor.h
          mdtcablelist/mdtClWireEditor.h
          mdtcablelist/mdtClLinkVersion.h
          mdtcablelist/mdtClUnitLinkDialog.h
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.h
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.h
          mdtcablelist/mdtClLinkBeamEditor.h
          mdtcablelist/mdtClConnectableConnectorDialog.h
    )

# Source files for mdtCableList
set (CL_SOURCES_FILES 
          mdtcablelist/mdtClApplicationWidgets.cpp
          mdtcablelist/mdtClModificationModel.cpp
          mdtcablelist/mdtClVehicleTypeEditor.cpp
          mdtcablelist/mdtClConnectionTypeData.cpp
          mdtcablelist/mdtClConnectionTypeModel.cpp
          mdtcablelist/mdtClConnectorData.cpp
          mdtcablelist/mdtClConnector.cpp
          mdtcablelist/mdtClConnectorEditor.cpp
          mdtcablelist/mdtClConnectorSelectionDialog.cpp
          mdtcablelist/mdtClConnectorContactSelectionDialog.cpp
          mdtcablelist/mdtClArticle.cpp
          mdtcablelist/mdtClArticleConnection.cpp
          mdtcablelist/mdtClArticleConnectorData.cpp
          mdtcablelist/mdtClArticleConnectorSelectionDialog.cpp
          mdtcablelist/mdtClArticleConnectionData.cpp
          mdtcablelist/mdtClArticleConnectionSelectionDialog.cpp
          mdtcablelist/mdtClArticleEditor.cpp
          mdtcablelist/mdtClArticleComponentDialog.cpp
          mdtcablelist/mdtClArticleConnectionDialog.cpp
          mdtcablelist/mdtClArticleLink.cpp
          mdtcablelist/mdtClArticleLinkDialog.cpp
          mdtcablelist/mdtClArticleSelectionDialog.cpp
          mdtcablelist/mdtClPathGraph.cpp
          mdtcablelist/mdtClPathGraphDialog.cpp
          mdtcablelist/mdtClPathGraphicsConnection.cpp
          mdtcablelist/mdtClPathGraphicsLink.cpp
          mdtcablelist/mdtClUnit.cpp
          mdtcablelist/mdtClUnitConnection.cpp
          mdtcablelist/mdtClUnitConnectorData.cpp
          mdtcablelist/mdtClUnitConnectorSelectionDialog.cpp
          mdtcablelist/mdtClUnitConnectionData.cpp
          mdtcablelist/mdtClUnitConnectionDialog.cpp
          mdtcablelist/mdtClUnitConnectionSelectionDialog.cpp
          mdtcablelist/mdtClUnitEditor.cpp
          mdtcablelist/mdtClWireEditor.cpp
          mdtcablelist/mdtClLinkTypeData.cpp
          mdtcablelist/mdtClLinkTypeModel.cpp
          mdtcablelist/mdtClLinkDirectionData.cpp
          mdtcablelist/mdtClLinkDirectionModel.cpp
          mdtcablelist/mdtClLinkData.cpp
          mdtcablelist/mdtClVehicleTypeLinkData.cpp
          mdtcablelist/mdtClLinkVersion.cpp
          mdtcablelist/mdtClLinkVersionModel.cpp
          mdtcablelist/mdtClLinkVersionDialog.cpp
          mdtcablelist/mdtClLink.cpp
          mdtcablelist/mdtClDirectLink.cpp
          mdtcablelist/mdtClUnitLinkDialog.cpp
          mdtcablelist/mdtClUnitVehicleType.cpp
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.cpp
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.cpp
          mdtcablelist/mdtClLinkBeam.cpp
          mdtcablelist/mdtClLinkBeamEditor.cpp
          mdtcablelist/mdtClConnectableConnectorDialog.cpp
    )

# Form files for mdtCableList
set (CL_FORM_FILES
          mdtcablelist/mdtClVehicleTypeEditor.ui
          mdtcablelist/mdtClConnectorEditor.ui
          mdtcablelist/mdtClArticleEditor.ui
          mdtcablelist/mdtClArticleComponentDialog.ui
          mdtcablelist/mdtClArticleConnectionDialog.ui
          mdtcablelist/mdtClArticleLinkDialog.ui
          mdtcablelist/mdtClPathGraphDialog.ui
          mdtcablelist/mdtClUnitConnectionDialog.ui
          mdtcablelist/mdtClUnitEditor.ui
          mdtcablelist/mdtClWireEditor.ui
          mdtcablelist/mdtClLinkVersionDialog.ui
          mdtcablelist/mdtClUnitLinkDialog.ui
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.ui
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.ui
          mdtcablelist/mdtClLinkBeamEditor.ui
          mdtcablelist/mdtClConnectableConnectorDialog.ui
    )

# Header files for complete test tool
set (HEADER_FILES ${TT_HEADER_FILES} ${CL_HEADER_FILES})

# Source files for complete test tool
set (SOURCES_FILES ${TT_SOURCES_FILES} ${CL_SOURCES_FILES})

# Form files for complete test tool
set (FORM_FILES ${TT_FORM_FILES} ${CL_FORM_FILES})

# Form files for complete test tool
set (RESOURCE_FILES  )


# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairies to link
add_library (mdttesttool${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdttools${MDTLIB_VERSION_SUFFIX}")

# Translations - We generate a .pro file, that simplify the usage of lupdate
file(WRITE "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "# Translations project file for lupdate/lrelease\n")
file(APPEND "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "# This file is generated by CMake, all changes will be lost\n")
file(APPEND "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "TRANSLATIONS = ${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool_fr.ts\n")
file(APPEND "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "TRANSLATIONS += ${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool_de.ts\n")
foreach(SRCFILE ${SOURCES_FILES})
  file(APPEND "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "SOURCES += ${PROJECT_SOURCE_DIR}/${SRCFILE}\n")
endforeach(SRCFILE)
foreach(FRMFILE ${FORM_FILES})
  file(APPEND "${PROJECT_SOURCE_DIR}/../../../translations/mdttesttool.pro" "FORMS += ${PROJECT_SOURCE_DIR}/${FRMFILE}\n")
endforeach(FRMFILE)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdttesttool" COMPONENT dev)

# Destinations
install(TARGETS "mdttesttool${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdttesttool${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libmdttesttool${MDTLIB_VERSION_SUFFIX}.so" "libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libmdttesttool${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg" "libmdttesttool${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
