# Project file for Multi Diag Tools

# Project
project(multidiagtools)

# List of source files
set(SOURCES_FILES
    src/Mdt/PlainText/FileInputIterator.cpp
    src/Mdt/PlainText/FileInputIteratorSharedData.cpp
)

# Headers directories
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../FilterExpression/src")
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ItemModel/src")
# # Because some headers are generated during building (config.h, Qt's UIC), we also build path
# include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library name
set(LIBRARY_NAME "PlainText_Core")

# Output library technical names
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LC)
set(LIBRARY_BIN_NAME "mdt${LIBRARY_NAME_LC}")
# Add output library
add_library(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterror${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} Qt5::Core)
# Headers directories
target_include_directories(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
# Because some headers are generated during building (config.h, Qt's UIC), we also add build path
target_include_directories(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
# Some specific definitions
# target_compile_definitions(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_de.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target("Mdt${LIBRARY_NAME}Ts" DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/Mdt/${LIBRARY_NAME}" COMPONENT dev)

# Install targets
install(TARGETS "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  mdtSeparateDebugSymbols(${LIBRARY_BIN_NAME})
endif(UNIX)

# Unit tests
add_subdirectory("tests")
