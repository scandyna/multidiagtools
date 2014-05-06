# Project file for Multi Diag Tools
# * Part: mdttesttool *
# * Directory: mdttools/mdttesttool *

# Project
project (multidiagtools)

# Header files for mdtTestTool
set (TT_HEADER_FILES 
          mdtTtTestConnectionCableEditor.h
          mdtTtTestLinkDialog.h
          mdtTtTestNodeEditor.h
          mdtTtTestNodeUnitDialog.h
    )

# Sources files for mdtTestTool
set (TT_SOURCES_FILES 
          mdtTtBase.cpp
          mdtTtDatabaseSchema.cpp
          mdtTtTestLinkData.cpp
          mdtTtTestLinkDialog.cpp
          mdtTtTestConnectionCable.cpp
          mdtTtTestConnectionCableEditor.cpp
          mdtTtTestNode.cpp
          mdtTtTestNodeEditor.cpp
          mdtTtTestNodeUnit.cpp
          mdtTtTestNodeUnitData.cpp
          mdtTtTestNodeUnitDialog.cpp
    )

# Form files for mdtTestTool
set (TT_FORM_FILES
          mdtTtTestConnectionCableEditor.ui
          mdtTtTestLinkDialog.ui
          mdtTtTestNodeEditor.ui
          mdtTtTestNodeUnitDialog.ui
    )

# Header files for mdtCableList
set (CL_HEADER_FILES 
          mdtcablelist/mdtClVehicleTypeEditor.h
          mdtcablelist/mdtClConnectorEditor.h
          mdtcablelist/mdtClArticleEditor.h
          mdtcablelist/mdtClArticleComponentDialog.h
          mdtcablelist/mdtClArticleConnectionDialog.h
          mdtcablelist/mdtClArticleLinkDialog.h
          mdtcablelist/mdtClPathGraphDialog.h
          mdtcablelist/mdtClUnitConnectionDialog.h
          mdtcablelist/mdtClUnitEditor.h
          mdtcablelist/mdtClUnitLinkDialog.h
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.h
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.h
    )

# Source files for mdtCableList
set (CL_SOURCES_FILES 
          mdtcablelist/mdtClVehicleTypeEditor.cpp
          mdtcablelist/mdtClConnectorData.cpp
          mdtcablelist/mdtClConnectorEditor.cpp
          mdtcablelist/mdtClArticle.cpp
          mdtcablelist/mdtClArticleConnectorData.cpp
          mdtcablelist/mdtClArticleConnectionData.cpp
          mdtcablelist/mdtClArticleEditor.cpp
          mdtcablelist/mdtClArticleComponentDialog.cpp
          mdtcablelist/mdtClArticleConnectionDialog.cpp
          mdtcablelist/mdtClArticleLinkDialog.cpp
          mdtcablelist/mdtClPathGraph.cpp
          mdtcablelist/mdtClPathGraphDialog.cpp
          mdtcablelist/mdtClPathGraphicsConnection.cpp
          mdtcablelist/mdtClPathGraphicsLink.cpp
          mdtcablelist/mdtClUnit.cpp
          mdtcablelist/mdtClUnitConnectorData.cpp
          mdtcablelist/mdtClUnitConnectionData.cpp
          mdtcablelist/mdtClUnitConnectionDialog.cpp
          mdtcablelist/mdtClUnitEditor.cpp
          mdtcablelist/mdtClLinkData.cpp
          mdtcablelist/mdtClVehicleTypeLinkData.cpp
          mdtcablelist/mdtClLink.cpp
          mdtcablelist/mdtClUnitLinkDialog.cpp
          mdtcablelist/mdtClUnitVehicleType.cpp
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.cpp
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.cpp
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
          mdtcablelist/mdtClUnitLinkDialog.ui
          mdtcablelist/mdtClLinkedUnitConnectionInfoDialog.ui
          mdtcablelist/mdtClLinkedUnitConnectorInfoDialog.ui
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
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtport${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtserialport${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttool${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")

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
# if(MDT_BUILD_STATIC_LIBS)
#   install(TARGETS "mdttesttool_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
# endif(MDT_BUILD_STATIC_LIBS)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdttesttool${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.so" "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg" "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libmdttesttool${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
