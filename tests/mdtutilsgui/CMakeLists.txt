# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutilsgui *
# * Directory: tests/mdutilsgui *

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

# * mdtIoWidgetTest executable *
QT4_WRAP_CPP(MDT_IO_WIDGET_TEST_MOC "mdtIoWidgetTest.h")
add_executable(mdtiowidgettest "mdtIoWidgetTest.cpp" ${MDT_IO_WIDGET_TEST_MOC})
target_link_libraries(mdtiowidgettest "mdtutils")
target_link_libraries(mdtiowidgettest "mdtutilsgui")
target_link_libraries(mdtiowidgettest "mdttest")
