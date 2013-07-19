# Project file for Multi Diag Tools
# * Part: mdudevice *
# * Directory: mdudevice *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # Device
          mdtDevice.h
          mdtDeviceIos.h
          mdtDeviceIosWidget.h
          mdtDeviceWindow.h
          #mdtDeviceStatusWidget.h
          mdtDeviceModbus.h
          mdtDeviceModbusWago.h
          mdtDeviceScpi.h
          mdtDeviceU3606A.h
          mdtDeviceDSO1000A.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          # Device
          mdtDevice.cpp
          mdtDeviceIos.cpp
          mdtDeviceIosWidget.cpp
          mdtDeviceWindow.cpp
          #mdtDeviceStatusWidget.cpp
          mdtDeviceModbus.cpp
          mdtDeviceModbusWago.cpp
          mdtDeviceScpi.cpp
          mdtDeviceU3606A.cpp
          mdtDeviceDSO1000A.cpp
    )

# Liste des fichiers ui
set (FORM_FILES
          mdtDeviceWindow.ui
    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

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

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtdevice${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevice${MDTLIB_VERSION_SUFFIX} ${QT_LIBRARIES})

# Version statique
# if(MDT_BUILD_STATIC_LIBS)
#   add_library (mdtdevice_static STATIC ${SOURCES_FILES} )
#   target_link_libraries(mdtdevice_static "mdtutils")
#   target_link_libraries(mdtdevice_static ${QT_LIBRARIES})
# endif(MDT_BUILD_STATIC_LIBS)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtdevice" COMPONENT dev)

# Destinations
install(TARGETS "mdtdevice${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtdevice${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.so" "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg" "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libmdtdevice${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)


#if(MDT_BUILD_STATIC_LIBS)
#  install(TARGETS "mdtdevice_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#endif(MDT_BUILD_STATIC_LIBS)
