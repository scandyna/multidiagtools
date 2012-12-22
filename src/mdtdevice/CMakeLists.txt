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
          mdtDeviceStatusWidget.h
          mdtDeviceModbus.h
          mdtDeviceScpi.h
          mdtDeviceU3606A.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          # Device
          mdtDevice.cpp
          mdtDeviceIos.cpp
          mdtDeviceIosWidget.cpp
          mdtDeviceWindow.cpp
          mdtDeviceStatusWidget.cpp
          mdtDeviceModbus.cpp
          mdtDeviceScpi.cpp
          mdtDeviceU3606A.cpp
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
  message("${PROJECT_SOURCE_DIR}/linux")
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
add_library (mdtdevice SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtdevice "mdtutils")
target_link_libraries(mdtdevice "mdtutilsgui")
target_link_libraries(mdtdevice "mdtport")
target_link_libraries(mdtdevice ${QT_LIBRARIES})

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
install(TARGETS "mdtdevice" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#if(MDT_BUILD_STATIC_LIBS)
#  install(TARGETS "mdtdevice_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#endif(MDT_BUILD_STATIC_LIBS)
