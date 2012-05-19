# CMake project file for Multi Diag Tools

# Project
project (multidiagtools)

# Access to project's include files
include_directories("${PROJECT_SOURCE_DIR}/../src")

# * UIC Number *
add_subdirectory("${PROJECT_SOURCE_DIR}/uicnumber")
