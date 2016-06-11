# Project file for Multi Diag Tools

# Project
project(multidiagtools)

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

# Output library name
set(LIBRARY_NAME "Expected")

# Output library technical names
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LC)
set(LIBRARY_BIN_NAME "mdt${LIBRARY_NAME_LC}")

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/Mdt/${LIBRARY_NAME}" COMPONENT dev)

# Unit tests
add_subdirectory("tests")
