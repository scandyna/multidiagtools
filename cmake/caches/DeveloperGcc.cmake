
set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

# Sanitizer
# NOTE: must be the first option, else undefined references will occur.
#       when using add_compile_options(), it is not at first,
#       so we use CMAKE_CXX_FLAGS (yes, a workaround)
set(MDT_CXX_FLAGS_TO_PREPEND "-fsanitize=address -fsanitize=undefined" CACHE STRING "Sanitizer options")
set(MDT_CXX_FLAGS_TO_APPEND "-O3 -Wall -Wextra" CACHE STRING "")

# On *NIX we use mult-arch install (using GNUInstallDirs)
# This only works if CMAKE_INSTALL_PREFIX is set to /usr
# Because default is to build installations package, default this to /usr
if(UNIX)
  set(CMAKE_INSTALL_PREFIX /usr CACHE FILE "Installation prefix")
endif()
