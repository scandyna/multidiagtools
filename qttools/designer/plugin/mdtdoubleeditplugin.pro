
CONFIG     += designer plugin qt
CONFIG     += debug_and_release
CONFIG     += release
CONFIG     += warn_on
TARGET      = $$qtLibraryTarget($$TARGET)
TEMPLATE    = lib
INCLUDEPATH += ../../../src/mdtutilsgui/

HEADERS     = mdtDoubleEditPlugin.h \
              ../../../src/mdtutilsgui/mdtDoubleEdit.h \
              ../../../src/mdtutilsgui/mdtDoubleValidator.h \

SOURCES     = mdtDoubleEditPlugin.cpp \
              ../../../src/mdtutilsgui/mdtDoubleEdit.cpp \
              ../../../src/mdtutilsgui/mdtDoubleValidator.cpp \

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS   += target
