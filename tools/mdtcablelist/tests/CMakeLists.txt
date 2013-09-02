# CMake project file for Multi X Diag Tools
# * Unit test for: mdtcablelist *
# * Directory: tools/mdtcablelist/tests/ *

# Project
project (multidiagtools)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# Header files containing Qt Q_OBJECT macro
set (HEADER_FILES
          ../src/mdtClVehicleTypeEditor.h
          ../src/mdtClArticleEditor.h
          ../src/mdtClUnitEditor.h
          ../src/mdtClLinkEditor.h
          ../src/mdtClUnitConnectionSelectionDialog.h
          ../src/mdtClTerminalBlockEditor.h
          ../src/mdtClTerminalEditorDialog.h
          ../src/mdtClArticleLinkDialog.h
    )

# Source files
set (SOURCES_FILES 
          ../src/mdtClVehicleTypeEditor.cpp
          ../src/mdtClArticleEditor.cpp
          ../src/mdtClUnit.cpp
          ../src/mdtClUnitEditor.cpp
          ../src/mdtClUnitConnectionData.cpp
          ../src/mdtClLinkEditor.cpp
          ../src/mdtClUnitConnectionSelectionDialog.cpp
          ../src/mdtClTerminalBlockEditor.cpp
          ../src/mdtClTerminalEditorDialog.cpp
          ../src/mdtClUnitVehicleType.cpp
          ../src/mdtClArticleLinkDialog.cpp
          ../src/mdtClArticle.cpp
    )

# Qt ui files
set (FORM_FILES
          ../src/mdtClVehicleTypeEditor.ui
          ../src/mdtClArticleEditor.ui
          ../src/mdtClUnitEditor.ui
          ../src/mdtClUnitConnectionEditor.ui
          ../src/mdtClLinkEditor.ui
          ../src/mdtClTerminalBlockEditor.ui
          ../src/mdtClArticleLinkDialog.ui
    )

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/../../../tests/mdttest")
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Sandbox
QT4_WRAP_CPP(MDT_CL_SANDBOX "mdtClSandbox.h")
add_executable(mdtclsandbox "mdtClSandbox.cpp" ${MDT_CL_SANDBOX})
target_link_libraries(mdtclsandbox "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdttest")

# mdtClUnit
QT4_WRAP_CPP(MDT_CL_UNIT_TEST "mdtClUnitTest.h")
add_executable(mdtclunittest "mdtClUnitTest.cpp" ${MDT_CL_UNIT_TEST} ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtclunittest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunittest "mdttest")
