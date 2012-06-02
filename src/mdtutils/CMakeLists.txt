# Project file for Multi Diag Tools
# * Part: mdtutils *
# * Directory: mdtutils *

# Project
project (multidiagtools)

# List of Qt headers (headers that contains Q_OBJECT macro)
set (HEADER_FILES 
          # mdtError
          mdtErrorOut.h
          # UIC Number
          mdtUicNumberWidget.h
    )

# List of source files
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
          # File
          mdtCsvFile.cpp
          # UIC number
          mdtUicNumber.cpp
          mdtUicNumberItem.cpp
          mdtUicNumberWidget.cpp
          mdtUicNumberValidator.cpp
          # mdtFileSystem
          mdtPartitionAttributes.cpp
          # Main application
          mdtApplication.cpp
    )

# List of .ui files
set (FORM_FILES
          mdtUicNumberWidget.ui
    )

# List of rcc files (Qt ressources)
set (RESOURCE_FILES

    )

# Platform specific files

# Unix (Posix) specific files
if(UNIX)
  set (SOURCES_FILES ${SOURCES_FILES}
          linux/mdtPartitionAttributesPosix.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(UNIX)

# Windows specific files
if(WIN32)
  set (SOURCES_FILES ${SOURCES_FILES}
          windows/mdtPartitionAttributesWindows.cpp
  )
  set (HEADER_FILES ${HEADER_FILES} 
  )
endif(WIN32)

# Files on witch moc compiler must work
QT4_WRAP_CPP (MOC_HEADERS ${HEADER_FILES})

# Files on witch uic compiler must work
QT4_WRAP_UI (UIC_HEADERS ${FORM_FILES})

# Files on witch rcc compiler must work
QT4_ADD_RESOURCES (RCC_FILES ${RESOURCE_FILES})

# Output library
add_library (mdtutils SHARED ${SOURCES_FILES} ${MOC_HEADERS} ${UIC_HEADERS} ${RCC_FILES})
target_link_libraries(mdtutils ${QT_LIBRARIES})
target_link_libraries(mdtutils "qtsingleapplication")

# Static version of the library
if(MDT_BUILD_STATIC_LIBS)
  add_library (mdtutils_static STATIC ${SOURCES_FILES} )
  target_link_libraries(mdtutils_static ${QT_LIBRARIES})
  set_target_properties("mdtutils_static" PROPERTIES OUTPUT_NAME "mdtutils")
endif(MDT_BUILD_STATIC_LIBS)

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/mdtutils" COMPONENT dev)

# Install targets
install(TARGETS "mdtutils" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
if(MDT_BUILD_STATIC_LIBS)
  install(TARGETS "mdtutils_static" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)
endif(MDT_BUILD_STATIC_LIBS)
