# CMake project file for Multi X Diag Tools
# * Unit test for: mdtcablelist *
# * Directory: tools/mdtcablelist/tests/ *

# Project
project (multidiagtools)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/../../../tests/mdttest")
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Sandbox
QT4_WRAP_CPP(MDT_CL_SANDBOX "mdtClSandbox.h")
add_executable(mdtclsandbox "mdtClSandbox.cpp" ${MDT_CL_SANDBOX})
target_link_libraries(mdtclsandbox "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclsandbox "mdttest")
