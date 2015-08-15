# Project file for Multi Diag Tools
# * Part: mdudevice *
# * Directory: mdudevice *

# Project
project (multidiagtools)

# Liste des fichiers sources
set (SOURCES_FILES 
          # Device
          mdtDevice.cpp
          mdtMultiIoDevice.cpp
          mdtDeviceContainer.cpp
          mdtDeviceContainerWidget.cpp
          mdtDeviceIos.cpp
          mdtDeviceIosSegment.cpp
          mdtDeviceIosWidget.cpp
          mdtDeviceWindow.cpp
          #mdtDeviceStatusWidget.cpp
          mdtDeviceModbus.cpp
          mdtDeviceModbusWago.cpp
          mdtDeviceModbusWagoModule.cpp
          mdtDeviceModbusWagoModuleRtd.cpp
          mdtDeviceScpi.cpp
          mdtDeviceU3606A.cpp
          mdtDeviceDSO1000A.cpp
    )


# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  #message("${PROJECT_SOURCE_DIR}/linux")
  set (SOURCES_FILES ${SOURCES_FILES} 
#    linux/mdtDeviceU3606AWidget.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
#    linux/mdtDeviceU3606AWidget.h
  )
  set (FORM_FILES ${FORM_FILES}
#    linux/mdtDeviceU3606AWidget.ui
  )

endif(UNIX)


# Librairie du projet
add_library (mdtdevice${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtdevice" COMPONENT dev)

# Destinations
install(TARGETS "mdtdevice${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtdevice${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "libmdtdevice${MDTLIB_VERSION_SUFFIX}.so" "libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "libmdtdevice${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg" "libmdtdevice${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
