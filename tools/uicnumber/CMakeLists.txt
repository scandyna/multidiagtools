# CMake project file for Multi Diag Tools
# Tool: mdtuicnumber

# Project
project (multidiagtools)

# Source files
set (SOURCES_FILES 
          # Main
          main.cpp
    )

# Executable
add_executable (mdtuicnumber${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES})
# Libraries to link
target_link_libraries(mdtuicnumber${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtuicnumber${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)

# Install targets
install (TARGETS "mdtuicnumber${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)
