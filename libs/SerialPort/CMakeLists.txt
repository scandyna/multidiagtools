# Project file for Multi Diag Tools
# * Part: SerialPort *

# Project
project(multidiagtools)

# List of source files
set(SOURCES_FILES 
    src/Mdt/SerialPort/SetupDialog.cpp
)

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
# Because some headers are generated during building (config.h, Qt's UIC), we also build path
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library
add_library (mdtserialport${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} Qt5::SerialPort)
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} "mdterrorOld${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} "mdterrorguiOld${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/mdtserialport_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/mdtserialport_de.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(mdtSerialPortTs DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtserialport" COMPONENT dev)

# Install targets
install(TARGETS "mdtserialport${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtserialport${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libmdtserialport${MDTLIB_VERSION_SUFFIX}.so" "libmdtserialport${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libmdtserialport${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtserialport${MDTLIB_VERSION_SUFFIX}.dbg" "libmdtserialport${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libmdtserialport${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libmdtserialport${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)

# Unit tests
add_subdirectory("tests")
