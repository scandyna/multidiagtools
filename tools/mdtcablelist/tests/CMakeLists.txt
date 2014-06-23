# CMake project file for Multi X Diag Tools
# * Unit test for: mdtcablelist *
# * Directory: tools/mdtcablelist/tests/ *

# Project
project (multidiagtools)

# message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# message("HDRs: ${HEADER_FILES}")

# Header files containing Qt Q_OBJECT macro
set (TEST_HEADER_FILES
          #../src/mdtClEditor.h
          #../src/mdtClVehicleTypeEditor.h
          #../src/mdtClConnectorEditor.h
          #../src/mdtClArticleEditor.h
          #../src/mdtClArticleConnectionDialog.h
#           ../src/mdtClUnitEditor.h
#           ../src/mdtClLinkEditor.h
#           ../src/mdtClUnitConnectionDialog.h
#           ../src/mdtClUnitConnectionSelectionDialog.h
#           ../src/mdtClUnitLinkDialog.h
          #../src/mdtClTerminalBlockEditor.h
          #../src/mdtClTerminalEditorDialog.h
          #../src/mdtClArticleComponentDialog.h
          #../src/mdtClArticleLinkDialog.h
          #../src/mdtClPathGraphDialog.h
          ../src/mdtClMainWindow.h
          #../src/mdtCcTestConnectionCableEditor.h
          #../src/mdtTtTestNodeEditor.h
          #../src/mdtTtTestModelEditor.h
          #../src/mdtTtTestModelItemEditor.h
          ../src/mdtTtCableChecker.h
          ../src/mdtTtCableCheckerWindow.h
          ../src/mdtTtTestNodeUnitEditor.h
   )

# Source files
set (TEST_SOURCES_FILES 
          #../src/mdtClBase.cpp
          #../src/mdtClEditor.cpp
          #../src/mdtClVehicleTypeEditor.cpp
          #../src/mdtClConnectorEditor.cpp
          ../src/mdtClConnectionData.cpp
#          ../src/mdtClArticleConnectionData.cpp
          #../src/mdtClArticleEditor.cpp
          #../src/mdtClArticleConnectionDialog.cpp
#           ../src/mdtClUnit.cpp
#           ../src/mdtClUnitEditor.cpp
#           ../src/mdtClUnitConnectionData.cpp
#           ../src/mdtClLinkEditor.cpp
#           ../src/mdtClUnitConnectionDialog.cpp
#           ../src/mdtClUnitConnectionSelectionDialog.cpp
#           ../src/mdtClUnitLinkDialog.cpp
#          ../src/mdtClLinkData.cpp
          #../src/mdtClTerminalBlockEditor.cpp
          #../src/mdtClTerminalEditorDialog.cpp
#           ../src/mdtClUnitVehicleType.cpp
          #../src/mdtClArticleComponentDialog.cpp
          #../src/mdtClArticleLinkDialog.cpp
          #../src/mdtClArticle.cpp
          #../src/mdtClPathGraphicsConnection.cpp
          #../src/mdtClPathGraphicsLink.cpp
          #../src/mdtClPathGraph.cpp
          #../src/mdtClPathGraphDialog.cpp
          #../src/mdtClDatabaseSchema.cpp
          ../src/mdtClMainWindow.cpp
          #../src/mdtCcTestConnectionCable.cpp
          #../src/mdtCcTestConnectionCableEditor.cpp
          #../src/mdtTtTestNodeEditor.cpp
          #../src/mdtTtTestNode.cpp
          #../src/mdtTtTestModelEditor.cpp
          #../src/mdtTtTestModel.cpp
          #../src/mdtTtTestModelItemEditor.cpp
          #../src/mdtTtTestModelItem.cpp
          ../src/mdtTtTest.cpp
          ../src/mdtTtCableChecker.cpp
          ../src/mdtTtCableCheckerWindow.cpp
          ../src/mdtTtTestNodeUnitSetupData.cpp
          #../src/mdtTtTestNodeUnitData.cpp
          ../src/mdtTtTestNodeUnitEditor.cpp
    )

# Qt ui files
set (TEST_FORM_FILES
          #../src/mdtClVehicleTypeEditor.ui
          #../src/mdtClConnectorEditor.ui
          #../src/mdtClArticleEditor.ui
          #../src/mdtClArticleConnectionDialog.ui
#           ../src/mdtClUnitEditor.ui
#           ../src/mdtClUnitConnectionDialog.ui
#           ../src/mdtClUnitLinkDialog.ui
#          ../src/mdtClUnitConnectionEditor.ui
#           ../src/mdtClLinkEditor.ui
          #../src/mdtClTerminalBlockEditor.ui
          #../src/mdtClArticleComponentDialog.ui
          #../src/mdtClArticleLinkDialog.ui
          #../src/mdtClPathGraphDialog.ui
          ../src/mdtClMainWindow.ui
          #../src/mdtTtTestNodeEditor.ui
          #../src/mdtTtTestModelEditor.ui
          #../src/mdtTtTestModelItemEditor.ui
          #../src/mdtCcTestConnectionCableEditor.ui
          ../src/mdtTtCableChecker.ui
          ../src/mdtTtCableCheckerWindow.ui
          ../src/mdtTtTestNodeUnitEditor.ui
    )

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/../../../tests/mdttest")
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Sandbox
QT4_WRAP_CPP(MDT_CL_SANDBOX_MOC "mdtClSandbox.h" ${TEST_HEADER_FILES})
add_executable(mdtclsandbox "mdtClSandbox.cpp" ${TEST_SOURCES_FILES} ${MDT_CL_SANDBOX_MOC})
target_link_libraries(mdtclsandbox "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclsandbox "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclsandbox "mdtdport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdttest")

# mdtClUnit
QT4_WRAP_CPP(TEST_MOC "mdtClUnitTest.h" ${TEST_HEADER_FILES})
add_executable(mdtclunittest "mdtClUnitTest.cpp" ${TEST_SOURCES_FILES} ${TEST_MOC})
target_link_libraries(mdtclunittest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclunittest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclunittest "mdtdport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdttest")
