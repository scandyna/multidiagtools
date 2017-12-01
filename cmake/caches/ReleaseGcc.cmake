
set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")

# Compiler options
# On MinGW, LTO seems not to be supported
# At least, it causes problem with the default distributed version
# Note: if(MINGW) does not work. (Gcc on Windows is MinGW)
if(WIN32)
  set(MDT_CXX_FLAGS_TO_APPEND "-O3 -Wall -Wextra -Werror" CACHE STRING "")
else()
  set(MDT_CXX_FLAGS_TO_APPEND "-O3 -Wall -Wextra -Werror -flto" CACHE STRING "")
endif()
