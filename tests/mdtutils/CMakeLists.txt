# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdutils *

# Project
project (multidiagtools)

# * mdtAlgorithmsTest executable *
QT4_WRAP_CPP(MDT_ALGORITHM_TEST_MOC "mdtAlgorithmsTest.h")
add_executable(mdtalgorithmstest "mdtAlgorithmsTest.cpp" ${MDT_ALGORITHM_TEST_MOC})
target_link_libraries(mdtalgorithmstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtalgorithmstest "mdttest")

# * mdtErrorTest executable *
QT4_WRAP_CPP(MDT_ERROR_TEST_MOC "mdtErrorTest.h")
add_executable(mdterrortest "mdtErrorTest.cpp" ${MDT_ERROR_TEST_MOC})
target_link_libraries(mdterrortest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortest "mdttest")

# * mdtBufferTest executable *
QT4_WRAP_CPP(MDT_BUFFER_TEST_MOC "mdtBufferTest.h")
add_executable(mdtbuffertest "mdtBufferTest.cpp" ${MDT_BUFFER_TEST_MOC})
target_link_libraries(mdtbuffertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtbuffertest "mdttest")

# * mdtFrameTest executable *
QT4_WRAP_CPP(MDT_FRAME_TEST_MOC "mdtFrameTest.h")
add_executable(mdtframetest "mdtFrameTest.cpp" ${MDT_FRAME_TEST_MOC})
target_link_libraries(mdtframetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtframetest "mdttest")

# * mdtFrameCodecTest executable *
QT4_WRAP_CPP(MDT_FRAME_CODEC_TEST_MOC "mdtFrameCodecTest.h")
add_executable(mdtframecodectest "mdtFrameCodecTest.cpp" ${MDT_FRAME_CODEC_TEST_MOC})
target_link_libraries(mdtframecodectest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtframecodectest "mdttest")

# * mdtCodecTest executable *
QT4_WRAP_CPP(MDT_CODEC_TEST_MOC "mdtCodecTest.h")
add_executable(mdtcodectest "mdtCodecTest.cpp" ${MDT_CODEC_TEST_MOC})
target_link_libraries(mdtcodectest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcodectest "mdttest")

# * mdtUicNumberTest executable *
QT4_WRAP_CPP(MDT_UIC_NUMBER_TEST_MOC "mdtUicNumberTest.h")
add_executable(mdtuicnumbertest "mdtUicNumberTest.cpp" ${MDT_UIC_NUMBER_TEST_MOC})
target_link_libraries(mdtuicnumbertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtuicnumbertest "mdttest")

# * mdtFileTest executable *
QT4_WRAP_CPP(MDT_FILE_TEST_MOC "mdtFileTest.h")
add_executable(mdtfiletest "mdtFileTest.cpp" ${MDT_FILE_TEST_MOC})
target_link_libraries(mdtfiletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfiletest "mdttest")

# * mdtValueTest executable *
QT4_WRAP_CPP(MDT_VALUE_TEST_MOC "mdtValueTest.h")
add_executable(mdtvaluetest "mdtValueTest.cpp" ${MDT_VALUE_TEST_MOC})
target_link_libraries(mdtvaluetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtvaluetest "mdttest")

# * mdtIoTest executable *
QT4_WRAP_CPP(MDT_IO_TEST_MOC "mdtIoTest.h")
add_executable(mdtiotest "mdtIoTest.cpp" ${MDT_IO_TEST_MOC})
target_link_libraries(mdtiotest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtiotest "mdttest")

# * mdtStateMachineTest executable *
QT4_WRAP_CPP(MDT_STATE_MACHINE_TEST_MOC "mdtStateMachineTest.h")
add_executable(mdtstatemachinetest "mdtStateMachineTest.cpp" ${MDT_STATE_MACHINE_TEST_MOC})
target_link_libraries(mdtstatemachinetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtstatemachinetest "mdttest")

