# CMake project file for Multi X Diag Tools
# * Part: mdtutils *
# * Directory: mdtserialport *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtAbstractSerialPort.h
          mdtSerialPort.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtSerialPortManager.cpp
          mdtAbstractSerialPort.cpp
          mdtSerialPort.cpp
          mdtSerialPortConfig.cpp
          mdtSerialPortCtlThread.cpp
    )

# Liste des fichiers ui
set (FORM_FILES

    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  message("${PROJECT_SOURCE_DIR}/linux")
  set (SOURCES_FILES ${SOURCES_FILES} 
    linux/mdtSerialPortPosix.cpp
  )
endif(UNIX)

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtserialport SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtserialport ${QT_LIBRARIES})

# Version statique
add_library (mdtserialport_static STATIC ${SOURCES_FILES} )
target_link_libraries(mdtserialport_static ${QT_LIBRARIES})
#set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")

# Destinations
install (TARGETS "mdtserialport" DESTINATION "${LIB_DEST_DIR}")
install (TARGETS "mdtserialport_static" DESTINATION "${LIB_DEST_DIR}")
