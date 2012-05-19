# CMake project file for Multi Diag Tools
# Tool: mdtuicnumber

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
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Executable
add_executable (mdtuicnumber ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
# Libraries to link
target_link_libraries(mdtuicnumber "mdtutils")
target_link_libraries(mdtuicnumber ${QT_LIBRARIES})

# Install targets
install (TARGETS "mdtuicnumber" RUNTIME DESTINATION bin)
