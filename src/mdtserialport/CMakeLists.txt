# CMake project file for Multi Diag Tools
# * Part: mdtserialport${MDTLIB_VERSION_SUFFIX} *
# * Directory: mdtserialport${MDTLIB_VERSION_SUFFIX} *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtAbstractSerialPort.h
          # mdtSerialPort.h
          mdtSerialPortConfigWidget.h
          mdtSerialPortSetupDialog.h
          mdtSerialPortCtlWidget.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtSerialPortManager.cpp
          mdtAbstractSerialPort.cpp
          # mdtSerialPort.cpp
          mdtSerialPortConfig.cpp
          mdtSerialPortCtlThread.cpp
          mdtSerialPortConfigWidget.cpp
          mdtSerialPortSetupDialog.cpp
          mdtSerialPortCtlWidget.cpp
    )

# Liste des fichiers ui
set (FORM_FILES
          mdtSerialPortConfigWidget.ui
          mdtSerialPortSetupDialog.ui
    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  message("${PROJECT_SOURCE_DIR}/linux")
  set (SOURCES_FILES ${SOURCES_FILES} 
    linux/mdtSerialPort.cpp
  )
endif(UNIX)

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtserialport${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialport${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtserialport" COMPONENT dev)

# Destinations
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
