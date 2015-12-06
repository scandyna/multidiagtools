# CMake project file for Multi Diag Tools
# * Unit test for: mdtcsv *
# * Directory: tests/mdtcsv *

# Project
project (multidiagtools)

# * mdtCsvTest executable *
add_executable(mdtcsvtest "mdtCsvTest.cpp")
target_link_libraries(mdtcsvtest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcsvtest "mdtcsv${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcsvtest "mdttest")
