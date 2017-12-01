#.rst:
# MdtInstallDirs
# --------------
#
# Define common variables for installation directories
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# Inclusion of this module defines the following variables:
#
# ``MDT_INSTALL_<dir>``
#
#   Destination for files of a given type.  This value may be passed to
#   the ``DESTINATION`` options of :command:`install` commands for the
#   corresponding file type.
#
# where ``<dir>`` is one of:
#
# ``BINDIR``
#   user executables (``bin``)
# ``LIBDIR``
#   object code libraries
#   on Windows, it will be set to ``bin``
#   on UNIX:
#   - if ``CMAKE_INSTALL_PREFIX`` is /usr , it will be set to ``LIBDIR`` defined by the GnuInstallDirs module
#   - else it will be set to ``lib``
# ``TRANSLATIONDIR``
#   translation files (typically QM files)
#   on UNIX, if ``CMAKE_INSTALL_PREFIX`` is /usr , it will be set to ``DATADIR/${PROJECT_NAME}/translations``,
#   where DATADIR is defined by the GnuInstallDirs module
#   for all other cases, it will be set to ``translations``

get_filename_component(MDT_INSTALL_DIRS_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
if( UNIX AND ("${MDT_INSTALL_DIRS_INSTALL_PREFIX}" STREQUAL "/usr") )
  include(GNUInstallDirs)
  message("PREFIX is /usr")
  set(MDT_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
  set(MDT_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
  set(MDT_INSTALL_TRANSLATIONDIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations")
else()
  set(MDT_INSTALL_BINDIR "bin")
  if(WIN32)
    set(MDT_INSTALL_LIBDIR "bin")
  else()
    set(MDT_INSTALL_LIBDIR "lib")
  endif()
  set(MDT_INSTALL_TRANSLATIONDIR "translations")
endif()

message("MDT_INSTALL_BINDIR: ${MDT_INSTALL_BINDIR}")
message("MDT_INSTALL_LIBDIR: ${MDT_INSTALL_LIBDIR}")
message("MDT_INSTALL_TRANSLATIONDIR: ${MDT_INSTALL_TRANSLATIONDIR}")

