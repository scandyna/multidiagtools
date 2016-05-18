# CMake project file for Multi Diag Tools
# * Unit test for: mdterror *
# * Directory: tests/mdterror *

# Project
project (multidiagtools)

# * mdtErrorTest executable *
add_executable(mdterrortestOld "mdtErrorTest.cpp")
target_link_libraries(mdterrortestOld "mdttest")
target_link_libraries(mdterrortestOld "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortestOld "mdterrorOld${MDTLIB_VERSION_SUFFIX}")

# * mdtErrorGuiTest executable *
add_executable(mdterrorguitestOld "mdtErrorGuiTest.cpp")
target_link_libraries(mdterrorguitestOld "mdttest")
target_link_libraries(mdterrorguitestOld "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrorguitestOld "mdterrorguiOld${MDTLIB_VERSION_SUFFIX}")

