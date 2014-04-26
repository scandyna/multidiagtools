# CMake project file for Multi Diag Tools
# Tool: mdtcablelistexe

# Project
project (multidiagtools)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Header files containing Qt Q_OBJECT macro
set (HEADER_FILES
          src/mdtClMainWindow.h
          src/mdtTtTestModelEditor.h
          src/mdtTtTestModelItemEditor.h
          src/mdtTtCableChecker.h
          src/mdtTtCableCheckerWindow.h
          src/mdtTtTestNodeUnitEditor.h
    )

# Source files
set (SOURCES_FILES 
          # Main
          src/main.cpp
          src/mdtClConnectionData.cpp
          src/mdtClMainWindow.cpp
          src/mdtTtTestModelEditor.cpp
          src/mdtTtTestModel.cpp
          src/mdtTtTestModelItemEditor.cpp
          src/mdtTtTestModelItem.cpp
          src/mdtTtTest.cpp
          src/mdtTtCableChecker.cpp
          src/mdtTtCableCheckerWindow.cpp
          src/mdtTtTestNodeUnitSetupData.cpp
          src/mdtTtTestNodeUnitEditor.cpp
    )

# Qt ui files
set (FORM_FILES
          src/mdtClMainWindow.ui
          src/mdtTtTestModelEditor.ui
          src/mdtTtTestModelItemEditor.ui
          src/mdtTtCableChecker.ui
          src/mdtTtCableCheckerWindow.ui
          src/mdtTtTestNodeUnitEditor.ui
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
add_executable (mdtcablelistexe${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraries to link
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtdport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})

# Install targets
install (TARGETS "mdtcablelistexe${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)

#################################################################################################################################################
# *** Unit tests section ***
#################################################################################################################################################

# ** Unit tests compilation **
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
