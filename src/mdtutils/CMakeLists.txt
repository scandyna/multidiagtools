# CMake project file for Multi X Diag Tools
# * Part: mdtutils *
# * Directory: mdutils *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # mdtError
          mdtErrorOut.h
          # Device file I/O
          mdtAbstractPort.h
          mdtPort.h
          mdtPortThread.h
          mdtPortReadThread.h
          mdtPortWriteThread.h
          # Port manager
          mdtPortManager.h
          # Essais
          mdtParentChildTableModel.h
          mdtParentChildTableItem.h
    )


# Liste des fichiers sources
set (SOURCES_FILES 
          mdtAlgorithms.cpp
          mdtBuffer.cpp
          # Frame
          mdtFrame.cpp
          mdtFrameAscii.cpp
          mdtTime.cpp
          # Frame codec
          mdtFrameCodec.cpp
          mdtFrameCodecAscii.cpp
          mdtFrameCodecScpi.cpp
          mdtFrameCodecScpiU3606A.cpp
          # mdtError
          mdtError.cpp
          mdtErrorOut.cpp
          # Port I/O
          mdtAbstractPort.cpp
          mdtPort.cpp
          mdtPortConfig.cpp
          mdtPortThread.cpp
          mdtPortReadThread.cpp
          mdtPortWriteThread.cpp
          # Port manager
          mdtPortManager.cpp
          # Essais
          mdtParentChildTableModel.cpp
          mdtParentChildTableItem.cpp
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
add_library (mdtutils SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtutils ${QT_LIBRARIES})

# Version statique
add_library (mdtutils_static STATIC ${SOURCES_FILES} )
target_link_libraries(mdtutils_static ${QT_LIBRARIES})
#set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")

# Destinations
install (TARGETS "mdtutils" DESTINATION "${LIB_DEST_DIR}")
install (TARGETS "mdtutils_static" DESTINATION "${LIB_DEST_DIR}")
