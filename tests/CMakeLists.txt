# CMake project file for Multi X Diag Tools
# * Unit test root file *
# * Directory: tests *


# Project
project (multidiagtools)

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/mdttest")
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Add the unit test tools directory
add_subdirectory("${PROJECT_SOURCE_DIR}/mdttest")

# * mdtutils *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtutils")

# * mdtdevice *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtdevice")

# * mdtserialport *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtport")

# * mdtserialport *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtserialport")