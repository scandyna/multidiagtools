# CMake project file for Multi Diag Tools
# Tool: mdtterm

# Project
project (multidiagtools)

# Header files containing Qt Q_OBJECT macro
set (HEADER_FILES 

    )

# Source files
set (SOURCES_FILES 
          # Main
          main.cpp
    )

# Fichiers à traiter par le compilateur moc
# QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
# QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
# QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Executable
add_executable (mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES})
# add_executable (mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraries to link
target_link_libraries(mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} "mdttools${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})
target_link_libraries(mdtmodbusiotool${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)

# Install targets
install (TARGETS "mdtmodbusiotool${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)
