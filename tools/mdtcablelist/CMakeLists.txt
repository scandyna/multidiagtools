# CMake project file for Multi Diag Tools
# Tool: mdtcablelistexe

# Project
project (multidiagtools)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
# For headers that are generated during build, add current build directory
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Source files
set (SOURCES_FILES 
          # Main
          src/main.cpp
          #src/mdtClConnectionData.cpp
          src/mdtClMainWindow.cpp
          #src/mdtTtTestModelEditor.cpp
          #src/mdtTtTestModel.cpp
          #src/mdtTtTestModelItemEditor.cpp
          #src/mdtTtTestModelItem.cpp
          #src/mdtTtTest.cpp
          src/mdtTtCableChecker.cpp
          src/mdtTtCableCheckerWindow.cpp
          #src/mdtTtTestNodeUnitSetupData.cpp
          #src/mdtTtTestNodeUnitEditor.cpp
    )

# Executable
add_executable (mdtcablelistexe${MDTLIB_VERSION_SUFFIX} ${GUI_TYPE} ${SOURCES_FILES})
# Libraries to link
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} "mdttesttool${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistexe${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)

# Install targets
install (TARGETS "mdtcablelistexe${MDTLIB_VERSION_SUFFIX}" RUNTIME DESTINATION bin COMPONENT tools)
