# Project file for Multi Diag Tools
# * Part: mdtutilsgui *
# * Directory: mdutilsgui *

# Project
project (multidiagtools)

# Qwt library
find_library(Qwt REQUIRED)
if(NOT DEFINED QWT_FOUND)
  if(WIN32)
    include_directories("C:/qwt")
    include_directories("C:/qwt-6.0.0")
    include_directories("C:/qwt-6.0.1")
    include_directories("C:/qwt-6.0.2")
  else(WIN32)
    include_directories("/usr/local/include/qwt")
    include_directories("/usr/include/qwt")
  endif(WIN32)
endif(NOT DEFINED QWT_FOUND)

# Liste des fichiers d'entêtes Qt (entêtes contenant Q_OBJECT)
set (HEADER_FILES 
          # mdtLed
          mdtLed.h
          mdtBlinkLed.h
          # I/O Widgets
          mdtAbstractIoWidget.h
          mdtAnalogInWidget.h
          mdtAnalogOutWidget.h
          mdtDigitalInWidget.h
          mdtDigitalOutWidget.h
          # Database
          mdtSqlTableModel.h
          mdtDataWidgetMapper.h
          # Essais
          mdtParentChildTableModel.h
          mdtParentChildTableItem.h
    )

# Liste des fichiers sources
set (SOURCES_FILES 
          # mdtLed
          mdtLed.cpp
          mdtBlinkLed.cpp
          # I/O Widgets
          mdtAbstractIoWidget.cpp
          mdtAnalogInWidget.cpp
          mdtAnalogOutWidget.cpp
          mdtDigitalInWidget.cpp
          mdtDigitalOutWidget.cpp
          # Database
          mdtSqlTableModel.cpp
          mdtDataWidgetMapper.cpp
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
add_library (mdtutilsgui SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtutilsgui ${QT_LIBRARIES})
target_link_libraries(mdtutilsgui "qwt")

# Version statique
# if(MDT_BUILD_STATIC_LIBS)
#   add_library (mdtutilsgui_static STATIC ${SOURCES_FILES} )
#   target_link_libraries(mdtutilsgui_static ${QT_LIBRARIES})
#   set_target_properties("mdtutilsgui_static" PROPERTIES OUTPUT_NAME "mdtutils")
# endif(MDT_BUILD_STATIC_LIBS)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtutilsgui" COMPONENT dev)

# Destinations
install(TARGETS "mdtutilsgui" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#if(MDT_BUILD_STATIC_LIBS)
#  install(TARGETS "mdtutilsgui_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
#endif(MDT_BUILD_STATIC_LIBS)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "mdtutilsgui"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${PROJECT_SOURCE_DIR}/libmdtutilsgui.so" "${PROJECT_SOURCE_DIR}/libmdtutilsgui.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${PROJECT_SOURCE_DIR}/libmdtutilsgui.so"
                    COMMAND objcopy --add-gnu-debuglink="libmdtutilsgui.dbg" "${PROJECT_SOURCE_DIR}/libmdtutilsgui.so"
                    COMMAND chmod -x "${PROJECT_SOURCE_DIR}/libmdtutilsgui.dbg")
  install(FILES "${PROJECT_SOURCE_DIR}/libmdtutilsgui.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)
