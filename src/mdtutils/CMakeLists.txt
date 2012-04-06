# Project file for Multi Diag Tools
# * Part: mdtutils *
# * Directory: mdutils *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # mdtError
          mdtErrorOut.h
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
          mdtFrameModbusTcp.cpp
          mdtTime.cpp
          # Frame codec
          mdtFrameCodec.cpp
          mdtFrameCodecAscii.cpp
          mdtFrameCodecScpi.cpp
          mdtFrameCodecScpiU3606A.cpp
          mdtFrameCodecModbus.cpp
          # mdtError
          mdtError.cpp
          mdtErrorOut.cpp
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
  set (SOURCES_FILES ${SOURCES_FILES} 
  )
  set (HEADER_FILES ${HEADER_FILES} 
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
if(MDT_BUILD_STATIC_LIBS)
  add_library (mdtutils_static STATIC ${SOURCES_FILES} )
  target_link_libraries(mdtutils_static ${QT_LIBRARIES})
  set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")
endif(MDT_BUILD_STATIC_LIBS)

# Destinations
install (TARGETS "mdtutils" DESTINATION "${LIB_DEST_DIR}")
if(MDT_BUILD_STATIC_LIBS)
  install (TARGETS "mdtutils_static" DESTINATION "${LIB_DEST_DIR}")
endif(MDT_BUILD_STATIC_LIBS)