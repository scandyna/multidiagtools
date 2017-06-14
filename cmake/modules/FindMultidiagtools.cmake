#=========================================================================================
# - Try to find Multidiagtools libraries
#
# Usage:
#  set(MULTIDIAGTOOLS_VERSION_SUFFIX x.y)
#  find_package(Multidiagtools [REQUIRED] COMPONENTS comp1 comp2 ...)
#   REQUIRED (Optional): if set, a failure will be generated if one (or more) component was not found
#   COMPONENTS (Mandatory): needed components. NOTE: current version of this script does not check dependencies
#
# Example:
#  set(MULTIDIAGTOOLS_VERSION_SUFFIX 0.3)
#  find_package(Multidiagtools REQUIRED COMPONENTS mdtutils mdtutilsgui)
#
# Once done, following variables are defined:
#  MULTIDIAGTOOLS_FOUND : All requested components are found
#  MULTIDIAGTOOLS_INCLUDE_DIRS : The Multidiagtools include directories
#  MULTIDIAGTOOLS_LIBRARIES : The libraries needed
#
# The include_directories() is called for each header directory, and it's not needed to do it.
#=========================================================================================

# Search library for each component
foreach(LIB ${Multidiagtools_FIND_COMPONENTS})
  # Search library - For UNIX, because CMake process standard paths, we not have to specify them
  find_library(LIB_PATH
		NAMES "${LIB}${MULTIDIAGTOOLS_VERSION_SUFFIX}"
		PATHS "$ENV{ProgramFiles}/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}" "C:/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}"
		DOC "Multidiagtools libraries"
	      )
  # Add to libraries list
  list(APPEND LIBRARIES "${LIB_PATH}")
endforeach(LIB ${Multidiagtools_FIND_COMPONENTS})


# Search headers directory for config.h
unset(HDR_DIR_PATH CACHE)
find_path(HDR_DIR_PATH
	  NAMES "config.h"
	  PATH_SUFFIXES "mdt/mdt${MULTIDIAGTOOLS_VERSION_SUFFIX}"
	  PATHS "/usr/local/include" "/usr/include" "$ENV{ProgramFiles}/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}" "C:/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}"
	  DOC "Header directory for ${HDR_DIR}"
	)
list(APPEND HEADERS ${HDR_DIR_PATH})

# Search headers directory for qtsolutions
unset(HDR_DIR_PATH CACHE)
find_path(HDR_DIR_PATH
	  NAMES "qtsingleapplication.h"
	  PATH_SUFFIXES "mdt/mdt${MULTIDIAGTOOLS_VERSION_SUFFIX}/qtsolutions"
	  PATHS "/usr/local/include" "/usr/include" "$ENV{ProgramFiles}/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}" "C:/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}"
	  DOC "Header directory for ${HDR_DIR}"
	)
list(APPEND HEADERS ${HDR_DIR_PATH})

# Search headers directories for requested components
foreach(HDR_DIR ${Multidiagtools_FIND_COMPONENTS})
  # Choose known existing headers regarding component
  # Set a default header file name to be shure that CMake not return a path if nothing was found
  set(HDR_FILE "mdtfake.h")
  # Look about mdtdevice component
  if("${HDR_DIR}" STREQUAL "mdtdevice")
    set(HDR_FILE "mdtDevice.h")
  endif("${HDR_DIR}" STREQUAL "mdtdevice")
  # Look about mdtport component
  if("${HDR_DIR}" STREQUAL "mdtport")
    set(HDR_FILE "mdtAbstractPort.h")
  endif("${HDR_DIR}" STREQUAL "mdtport")
  # Look about mdtserialport component
  if("${HDR_DIR}" STREQUAL "mdtserialport")
    set(HDR_FILE "mdtSerialPortConfig.h")
  endif("${HDR_DIR}" STREQUAL "mdtserialport")
  # Look about mdttools component
  if("${HDR_DIR}" STREQUAL "mdttools")
    set(HDR_FILE "mdtPortTerm.h")
  endif("${HDR_DIR}" STREQUAL "mdttools")
  # Look about mdtutils component
  if("${HDR_DIR}" STREQUAL "mdtutils")
    set(HDR_FILE "mdtAlgorithms.h")
  endif("${HDR_DIR}" STREQUAL "mdtutils")
  # Look about mdtutilsgui component
  if("${HDR_DIR}" STREQUAL "mdtutilsgui")
    set(HDR_FILE "mdtLed.h")
  endif("${HDR_DIR}" STREQUAL "mdtutilsgui")
  # Search path
  unset(HDR_DIR_PATH CACHE)
  find_path(HDR_DIR_PATH
	    NAMES ${HDR_FILE}
	    PATH_SUFFIXES "mdt/mdt${MULTIDIAGTOOLS_VERSION_SUFFIX}/${HDR_DIR}"
	    PATHS "/usr/local/include" "/usr/include" "$ENV{ProgramFiles}/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}" "C:/multidiagtools${MULTIDIAGTOOLS_VERSION_SUFFIX}"
	    DOC "Header directory for ${HDR_DIR}"
	  )
  list(APPEND HEADERS ${HDR_DIR_PATH})
endforeach(HDR_DIR ${Multidiagtools_FIND_COMPONENTS})

list(REMOVE_DUPLICATES HEADERS)

# handle the QUIETLY and REQUIRED arguments and set MULTIDIAGTOOLS_FOUND to TRUE
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Multidiagtools DEFAULT_MSG LIBRARIES)
find_package_handle_standard_args(Multidiagtools DEFAULT_MSG HEADERS)

if(MULTIDIAGTOOLS_FOUND)
  # Libraries
  set(MULTIDIAGTOOLS_LIBRARIES ${LIBRARIES})
  foreach(LIB_DIR ${LIBRARIES})
    link_directories("${LIB_DIR}")
  endforeach(LIB_DIR ${LIBRARIES})
  # Headers
  set(MULTIDIAGTOOLS_INCLUDE_DIRS ${HEADERS})
  foreach(HDR_DIR ${HEADERS})
    include_directories("${HDR_DIR}")
  endforeach(HDR_DIR ${HEADERS})
  
endif(MULTIDIAGTOOLS_FOUND)
