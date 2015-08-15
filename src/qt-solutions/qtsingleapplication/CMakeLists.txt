# Project file for Multi Diag Tools

# Project
project (multidiagtools)

# List of source files
set (SOURCES_FILES 
          qtlocalpeer.cpp
          qtsingleapplication.cpp
    )


# Output library
add_library (qtsingleapplication${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(qtsingleapplication${MDTLIB_VERSION_SUFFIX} Qt5::Widgets Qt5::Network)

# Headers to install
file(GLOB INSTALL_HEADERS *.h QtSingleApplication QtLockedFile)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/qtsolutions" COMPONENT dev)

# Install targets
install(TARGETS "qtsingleapplication${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "qtsingleapplication${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.so" "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.dbg" "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libqtsingleapplication${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
