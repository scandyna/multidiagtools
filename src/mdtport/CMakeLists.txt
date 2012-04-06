# Project file for Multi Diag Tools
# * Part: mdtport *
# * Directory: mdtport *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # Port I/O
          mdtAbstractPort.h
          mdtPort.h
          mdtPortThread.h
          mdtPortReadThread.h
          mdtPortWriteThread.h
          mdtTcpSocket.h
          mdtTcpSocketThread.h
          # Port manager
          mdtPortManager.h
    )

# Liste des fichiers sources
set (SOURCES_FILES 
          # Port I/O
          mdtAbstractPort.cpp
          mdtPort.cpp
          mdtPortConfig.cpp
          mdtPortThread.cpp
          mdtPortReadThread.cpp
          mdtPortWriteThread.cpp
          mdtTcpSocket.cpp
          mdtTcpSocketThread.cpp
          # Port manager
          mdtPortManager.cpp
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
    linux/mdtPortPosix.cpp
    linux/mdtUsbtmcPort.cpp
    linux/mdtUsbtmcPortManager.h
    linux/mdtUsbtmcPortManager.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
    linux/mdtUsbtmcPort.h
    linux/mdtUsbtmcPortManager.h
  )
endif(UNIX)

# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdtport SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtport ${QT_LIBRARIES})

# Version statique
if(MDT_BUILD_STATIC_LIBS)
  add_library (mdtport_static STATIC ${SOURCES_FILES} )
  target_link_libraries(mdtport_static ${QT_LIBRARIES})
  set_target_properties("mdtport_static" PROPERTIES OUTPUT_NAME "mdtport")
endif(MDT_BUILD_STATIC_LIBS)

# Destinations
install (TARGETS "mdtport" DESTINATION "${LIB_DEST_DIR}")
if(MDT_BUILD_STATIC_LIBS)
  install (TARGETS "mdtport_static" DESTINATION "${LIB_DEST_DIR}")
endif(MDT_BUILD_STATIC_LIBS)