# Project file for Multi Diag Tools
# * Part: CableList *

# Project
project(multidiagtools)

# List of source files
# set(SOURCES_FILES
# )

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
# Because some headers are generated during building (config.h, Qt's UIC), we also build path
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library
# add_library(mdtnumeric${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
# target_link_libraries(mdtnumeric${MDTLIB_VERSION_SUFFIX} "mdterror${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtnumeric${MDTLIB_VERSION_SUFFIX} "mdterrorgui${MDTLIB_VERSION_SUFFIX}")

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/mdtnumeric_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/mdtnumeric_de.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(MdtNumericTs DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtnumeric" COMPONENT dev)

# Install targets
# install(TARGETS "mdtnumeric${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
# if(UNIX)
#   add_custom_command(TARGET "mdtnumeric${MDTLIB_VERSION_SUFFIX}"
#                     POST_BUILD
#                     COMMAND objcopy --only-keep-debug "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.so" "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.dbg"
#                     COMMAND strip --strip-debug --strip-unneeded "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.so"
#                     COMMAND objcopy --add-gnu-debuglink="libmdtnumeric${MDTLIB_VERSION_SUFFIX}.dbg" "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.so"
#                     COMMAND chmod -x "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.dbg")
#   install(FILES "libmdtnumeric${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
# endif(UNIX)

# Unit tests
add_subdirectory("tests")
