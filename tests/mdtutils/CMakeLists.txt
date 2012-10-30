# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdutils *

# Project
project (multidiagtools)

# * mdtAlgorithmsTest executable *
QT4_WRAP_CPP(MDT_ALGORITHM_TEST_MOC "mdtAlgorithmsTest.h")
add_executable(mdtalgorithmstest "mdtAlgorithmsTest.cpp" ${MDT_ALGORITHM_TEST_MOC})
target_link_libraries(mdtalgorithmstest "mdtutils")
target_link_libraries(mdtalgorithmstest "mdttest")

# * mdtErrorTest executable *
QT4_WRAP_CPP(MDT_ERROR_TEST_MOC "mdtErrorTest.h")
add_executable(mdterrortest "mdtErrorTest.cpp" ${MDT_ERROR_TEST_MOC})
target_link_libraries(mdterrortest "mdtutils")
target_link_libraries(mdterrortest "mdttest")

# * mdtBufferTest executable *
QT4_WRAP_CPP(MDT_BUFFER_TEST_MOC "mdtBufferTest.h")
add_executable(mdtbuffertest "mdtBufferTest.cpp" ${MDT_BUFFER_TEST_MOC})
target_link_libraries(mdtbuffertest "mdtutils")
target_link_libraries(mdtbuffertest "mdttest")

# * mdtFrameTest executable *
QT4_WRAP_CPP(MDT_FRAME_TEST_MOC "mdtFrameTest.h")
add_executable(mdtframetest "mdtFrameTest.cpp" ${MDT_FRAME_TEST_MOC})
target_link_libraries(mdtframetest "mdtutils")
target_link_libraries(mdtframetest "mdttest")

# * mdtFrameCodecTest executable *
QT4_WRAP_CPP(MDT_FRAME_CODEC_TEST_MOC "mdtFrameCodecTest.h")
add_executable(mdtframecodectest "mdtFrameCodecTest.cpp" ${MDT_FRAME_CODEC_TEST_MOC})
target_link_libraries(mdtframecodectest "mdtutils")
target_link_libraries(mdtframecodectest "mdttest")

# * mdtUicNumberTest executable *
QT4_WRAP_CPP(MDT_UIC_NUMBER_TEST_MOC "mdtUicNumberTest.h")
add_executable(mdtuicnumbertest "mdtUicNumberTest.cpp" ${MDT_UIC_NUMBER_TEST_MOC})
target_link_libraries(mdtuicnumbertest "mdtutils")
target_link_libraries(mdtuicnumbertest "mdttest")

# * mdtDatabaseTest executable *
QT4_WRAP_CPP(MDT_DATABASE_TEST_MOC "mdtDatabaseTest.h")
add_executable(mdtdatabasetest "mdtDatabaseTest.cpp" ${MDT_DATABASE_TEST_MOC})
target_link_libraries(mdtdatabasetest "mdtutils")
target_link_libraries(mdtdatabasetest "mdtutilsgui")
target_link_libraries(mdtdatabasetest "mdttest")

# * mdtFileTest executable *
QT4_WRAP_CPP(MDT_FILE_TEST_MOC "mdtFileTest.h")
add_executable(mdtfiletest "mdtFileTest.cpp" ${MDT_FILE_TEST_MOC})
target_link_libraries(mdtfiletest "mdtutils")
target_link_libraries(mdtfiletest "mdttest")

# * mdtIoTest executable *
QT4_WRAP_CPP(MDT_IO_TEST_MOC "mdtIoTest.h")
add_executable(mdtiotest "mdtIoTest.cpp" ${MDT_IO_TEST_MOC})
target_link_libraries(mdtiotest "mdtutils")
target_link_libraries(mdtiotest "mdttest")
