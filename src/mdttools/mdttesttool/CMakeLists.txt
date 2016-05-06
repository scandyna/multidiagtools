# Project file for Multi Diag Tools
# * Part: mdttesttool *
# * Directory: mdttools/mdttesttool *

# Project
project (multidiagtools)

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

# Source files for mdtCableList
set (CL_SOURCES_FILES 
          mdtcablelist/mdtClApplicationWidgets.cpp
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
          mdtcablelist/mdtClLinkData.cpp
          mdtcablelist/mdtClVehicleTypeLinkData.cpp
          mdtcablelist/mdtClVehicleTypeLinkAssignationWidget.cpp
          mdtcablelist/mdtClVehicleTypeCheckBox.cpp
          mdtcablelist/mdtClLinkVersion.cpp
          mdtcablelist/mdtClLinkVersionModel.cpp
          mdtcablelist/mdtClLinkVersionDialog.cpp
          mdtcablelist/mdtClLink.cpp
          mdtcablelist/mdtClVehicleTypeLink.cpp
          mdtcablelist/mdtClDirectLink.cpp
          mdtcablelist/mdtClUnitLinkDialog.cpp
          mdtcablelist/mdtClLinkAttributeDialog.cpp
          mdtcablelist/mdtClUnitVehicleType.cpp
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.cpp
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.cpp
          mdtcablelist/mdtClLinkBeam.cpp
          mdtcablelist/mdtClLinkBeamEditor.cpp
          mdtcablelist/mdtClAutoConnection.cpp
          mdtcablelist/mdtClConnectableConnectorDialog.cpp
    )

# # Source files for complete test tool
set (SOURCES_FILES ${TT_SOURCES_FILES} ${CL_SOURCES_FILES})

# Librairies to link
add_library (mdttesttool${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdttools${MDTLIB_VERSION_SUFFIX}")
# NOTE: used during mibration to libs
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

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
