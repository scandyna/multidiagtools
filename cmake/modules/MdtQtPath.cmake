#.rst:
# MdtQtPath
# --------------
#
# Get the prefix of the Qt library on which a Mdt installation depends.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# Inclusion of this module defines the following variables:
#
# ``QT_PREFIX_PATH``
#
#   Path to the Qt library on which a Mdt installation depends.
#

# list(LENGTH CMAKE_PREFIX_PATH _cmake_prefix_path_length)
# if(${_cmake_prefix_path_length} LESS 1)
#   message(FATAL_ERROR "")
# endif()

find_file(
  _qt_prefix_path_file
  NAMES "QtPrefixPath.txt"
  PATHS ${CMAKE_PREFIX_PATH}
  NO_DEFAULT_PATH
)

if(NOT EXISTS "${_qt_prefix_path_file}")
  message(FATAL_ERROR "Could not locate QtPrefixPath.txt. Searched in: ${CMAKE_PREFIX_PATH}")
endif()

file(STRINGS "${_qt_prefix_path_file}" QT_PREFIX_PATH)

# Check that QT_PREFIX_PATH points to a Qt installation
if(NOT QT_PREFIX_PATH)
  message(FATAL_ERROR "QT_PREFIX_PATH was not set, please check that CMAKE_PREFIX_PATH contains a path to a Mdt installation.")
endif()

# Add QT_PREFIX_PATH to CMAKE_PREFIX_PATH
list(FIND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH}" _qt_prefix_path_index)
if(${_qt_prefix_path_index} LESS 0)
  list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH}")
endif()

message("QT_PREFIX_PATH: ${QT_PREFIX_PATH}")
