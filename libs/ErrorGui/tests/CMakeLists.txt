# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtErrorGuiTest
add_executable(mdterrorguitest src/ErrorGuiTest.cpp)
target_link_libraries(mdterrorguitest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrorguitest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrorguitest Qt5::Test)
add_test(MdtErrorGuiTest ${CMAKE_CURRENT_BINARY_DIR}/mdterrorguitest)
