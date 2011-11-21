# CMake project file for Multi X Diag Tools
# * Unit test root file *
# * Directory: tests *


# Project
project (multixdiagtools)

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/mdttest")
include_directories("${PROJECT_SOURCE_DIR}/../src")

# Add the unit test tools directory
add_subdirectory("${PROJECT_SOURCE_DIR}/mdttest")

# * mdtutils *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtutils")

# * mdtserialport *
add_subdirectory("${PROJECT_SOURCE_DIR}/mdtserialport")