# Project file for Multi Diag Tools
# * Part: mdtutils *
# * Directory: mdtutils *

# Project
project (multidiagtools)

# List of Qt headers (headers that contains Q_OBJECT macro)
set (HEADER_FILES 
          qtlocalpeer.h
          qtsingleapplication.h
    )

# List of source files
set (SOURCES_FILES 
          qtlocalpeer.cpp
          qtlockedfile.cpp
          qtsingleapplication.cpp
    )

# List of .ui files
set (FORM_FILES
    )

# List of rcc files (Qt ressources)
set (RESOURCE_FILES

    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  set (SOURCES_FILES ${SOURCES_FILES} 
          qtlockedfile_unix.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(UNIX)

# Windows specific files
if(WIN32)
  set (SOURCES_FILES ${SOURCES_FILES} 
          qtlockedfile_win.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(WIN32)

# Files on witch moc compiler must work
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Files on witch uic compiler must work
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Files on witch rcc compiler must work
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Output library
add_library (qtsingleapplication SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(qtsingleapplication ${QT_LIBRARIES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h QtSingleApplication QtLockedFile)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/qtsolutions" COMPONENT dev)

# Install targets
install(TARGETS "qtsingleapplication" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "qtsingleapplication"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libqtsingleapplication.so" "${PROJECT_SOURCE_DIR}/libqtsingleapplication.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libqtsingleapplication.so"
                    COMMAND objcopy --add-gnu-debuglink="libqtsingleapplication.dbg" "${PROJECT_SOURCE_DIR}/libqtsingleapplication.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libqtsingleapplication.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libqtsingleapplication.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
