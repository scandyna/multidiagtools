# - Try to find qwt
# Once done this will define
#  QWT_FOUND - System has qwt
#  QWT_INCLUDE_DIRS - The qwt include directories
#  QWT_LIBRARIES - The libraries needed to use qwt
#=========================================================================================

# Find the include directory

if(WIN32)
  find_path(QWT_INCLUDE_DIR
	    NAMES qwt.h
	    PATHS "/qwt-6.0.3/include" "/qwt-6.0.2/include" "/qwt-6.0.1/include" "/qwt-6.0.0/include" "/qwt/include" "C:/qwt-6.0.3/include" "C:/qwt-6.0.2/include" "C:/qwt-6.0.1/include" "C:/qwt-6.0.0/include" "C:/qwt/include"
	    DOC "Qwt include directory"
	   )
endif(WIN32)

if(UNIX)
  find_path(QWT_INCLUDE_DIR
	    NAMES qwt.h
	    PATH_SUFFIXES qwt
	    DOC "Qwt include directory"
	   )
endif(UNIX)

# Find libraries

if(WIN32)
  find_library(QWT_LIBRARY
	    NAMES qwt qwtmathml
	    PATHS "/qwt-6.0.3/lib" "/qwt-6.0.2/lib" "/qwt-6.0.1/lib" "/qwt-6.0.0/lib" "/qwt/lib" "C:/qwt-6.0.3/lib" "C:/qwt-6.0.2/lib" "C:/qwt-6.0.1/lib" "C:/qwt-6.0.0/lib" "C:/qwt/lib"
	    DOC "Qwt libraries"
	   )
endif(WIN32)

if(UNIX)
  find_library(QWT_LIBRARY
	    NAMES qwt qwt-qt5 qwtmathml qwtmathml-qt5
	    DOC "Qwt libraries"
	   )
endif(UNIX)

# Define variables

set(QWT_INCLUDE_DIRS "${QWT_INCLUDE_DIR}")
set(QWT_LIBRARIES "${QWT_LIBRARY}")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Qwt DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR)

mark_as_advanced(QWT_INCLUDE_DIR QWT_LIBRARY )
