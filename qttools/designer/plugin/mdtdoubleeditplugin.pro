
CONFIG     += designer plugin qt
CONFIG     += debug_and_release
CONFIG     += release
CONFIG     += warn_on
QMAKE_CXXFLAGS += -std=c++11
TARGET      = $$qtLibraryTarget($$TARGET)
TEMPLATE    = lib
INCLUDEPATH += ../../../src/mdtutilsgui/
INCLUDEPATH += ../../../src/mdtutils/

HEADERS     = mdtDoubleEditPlugin.h \
              ../../../src/mdtutilsgui/mdtDoubleEdit.h \
              ../../../src/mdtutilsgui/mdtDoubleValidator.h \

SOURCES     = mdtDoubleEditPlugin.cpp \
              ../../../src/mdtutilsgui/mdtDoubleEdit.cpp \
              ../../../src/mdtutilsgui/mdtDoubleValidator.cpp \
              ../../../src/mdtutils/mdtAlgorithms.cpp \

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS   += target
