# CMake project file for Multi Diag Tools
# Tool: mdtterm

# Project
project (multidiagtools)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Header files containing Qt Q_OBJECT macro
set (HEADER_FILES 
          src/mdtClUnitEditor.h
    )

# Source files
set (SOURCES_FILES 
          # Main
          src/main.cpp
          src/mdtClUnitEditor.cpp
    )

# Qt ui files
set (FORM_FILES
          src/mdtClUnitEditor.ui
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
target_link_libraries(mdtcablelist${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})

# Install targets
install (TARGETS "mdtcablelist${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)
