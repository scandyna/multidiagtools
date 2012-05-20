# Project file for Multi Diag Tools
# * Part: mduttools *
# * Directory: mduttools *

# Project
project (multidiagtools)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          mdtPortTerm.h
    )

# Liste des fichiers sources
set (SOURCES_FILES 
          mdtPortTerm.cpp
    )

# Liste des fichiers ui
set (FORM_FILES
          mdtPortTerm.ui
    )

# Liste des fichiers rcc (ressources)
set (RESOURCE_FILES

    )


# Fichiers à traiter par le compilateur moc
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Fichiers à traiter par le compilateur uic
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Fichiers à traiter par le compilateur rcc
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Librairie du projet
add_library (mdttools SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdttools ${QT_LIBRARIES})
target_link_libraries(mdttools "mdtutils")
target_link_libraries(mdttools "mdtutilsgui")
target_link_libraries(mdttools "mdtport")
target_link_libraries(mdttools "mdtserialport")

# Version statique
if(MDT_BUILD_STATIC_LIBS)
  add_library (mdttools_static STATIC ${SOURCES_FILES} )
  target_link_libraries(mdttools_static ${QT_LIBRARIES})
  target_link_libraries(mdttools_static "mdtutils")
  target_link_libraries(mdttools_static "mdtutilsgui")
  target_link_libraries(mdttools_static "mdtport")
  target_link_libraries(mdttools_static "mdtserialport")
  set_target_properties("mdttools_static" PROPERTIES OUTPUT_NAME "mdttools")
endif(MDT_BUILD_STATIC_LIBS)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdttools" COMPONENT dev)

# Destinations
install(TARGETS "mdttools" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
if(MDT_BUILD_STATIC_LIBS)
  install(TARGETS "mdttools_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
endif(MDT_BUILD_STATIC_LIBS)
