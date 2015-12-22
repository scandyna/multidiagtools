# CMake project file for Multi Diag Tools
# * Unit test for: mdterror *
# * Directory: tests/mdterror *

# Project
project (multidiagtools)

# * mdtErrorTest executable *
add_executable(mdterrortest "mdtErrorTest.cpp")
target_link_libraries(mdterrortest "mdttest")
target_link_libraries(mdterrortest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortest "mdterror${MDTLIB_VERSION_SUFFIX}")

# * mdtErrorGuiTest executable *
add_executable(mdterrorguitest "mdtErrorGuiTest.cpp")
target_link_libraries(mdterrorguitest "mdttest")
target_link_libraries(mdterrorguitest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrorguitest "mdterrorgui${MDTLIB_VERSION_SUFFIX}")

