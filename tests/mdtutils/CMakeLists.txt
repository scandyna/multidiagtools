# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdutils *

# Project
project (multidiagtools)

# * mdtAlgorithmsTest executable *
add_executable(mdtalgorithmstest "mdtAlgorithmsTest.cpp")
target_link_libraries(mdtalgorithmstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtalgorithmstest "mdttest")

# * mdtMathTest executable *
add_executable(mdtmathtest "mdtMathTest.cpp")
target_link_libraries(mdtmathtest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtmathtest "mdttest")
# NOTE: only for sandbox
target_link_libraries(mdtmathtest "${QWT_LIBRARIES}")

# * mdtBufferTest executable *
add_executable(mdtbuffertest "mdtBufferTest.cpp")
target_link_libraries(mdtbuffertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtbuffertest "mdttest")

# * mdtFrameTest executable *
add_executable(mdtframetest "mdtFrameTest.cpp")
target_link_libraries(mdtframetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtframetest "mdttest")

# * mdtFrameCodecTest executable *
add_executable(mdtframecodectest "mdtFrameCodecTest.cpp")
target_link_libraries(mdtframecodectest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtframecodectest "mdttest")

# * mdtCodecTest executable *
add_executable(mdtcodectest "mdtCodecTest.cpp")
target_link_libraries(mdtcodectest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcodectest "mdttest")

# * mdtUicNumberTest executable *
add_executable(mdtuicnumbertest "mdtUicNumberTest.cpp")
target_link_libraries(mdtuicnumbertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtuicnumbertest "mdttest")

# * mdtFileTest executable *
add_executable(mdtfiletest "mdtFileTest.cpp")
target_link_libraries(mdtfiletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfiletest "mdttest")

# * mdtValueTest executable *
add_executable(mdtvaluetest "mdtValueTest.cpp")
target_link_libraries(mdtvaluetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtvaluetest "mdttest")

# * mdtIoTest executable *
add_executable(mdtiotest "mdtIoTest.cpp")
target_link_libraries(mdtiotest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtiotest "mdttest")

# * mdtStateMachineTest executable *
add_executable(mdtstatemachinetest "mdtStateMachineTest.cpp")
target_link_libraries(mdtstatemachinetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtstatemachinetest "mdttest")
