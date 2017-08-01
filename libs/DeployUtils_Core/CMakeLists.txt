
# TODO
# On Windows, we must have all required libraries available
# in the same directory than executables (tests, final tools)
# The final goal is that our tools makes the majority of the work.
#
# Steps:
#  - Copy needed functions from MdtDependenciesUtils.cmake here
#  - Copy needed libs (can do it by hand, 
#     with expericnce we will know what to copy for which compiler/arch)
#  - Once all runs good (hope it will happen some day..), update functions in MdtDependenciesUtils.cmake
#    and also the main CMakeLists.txt .

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(translations)
