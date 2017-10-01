
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    CommandLineParser.cpp
    MdtCpBinDepsMain.cpp
    main.cpp
)

# If we do cross-compilation from Linux -> Windows,
# we must run the deploy utils on Linux

message("CMAKE_HOST_SYSTEM_NAME : ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_VERSION : ${CMAKE_HOST_SYSTEM_VERSION}")
message("CMAKE_HOST_SYSTEM_PROCESSOR : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM : ${CMAKE_HOST_SYSTEM}")

# # When cross-compiling, import the deploy utils executables from a file
# if(CMAKE_CROSSCOMPILING)
#   set(IMPORT_DEPLOY_UTILS "IMPORTFILE-NOTFOUND" CACHE FILEPATH "")
#   include(${IMPORT_DEPLOY_UTILS})
# endif()

# Only build deploy utils if we do a native build
if(NOT CMAKE_CROSSCOMPILING)
  add_executable(mdtcpbindeps ${SOURCE_FILES})
  target_link_libraries(mdtcpbindeps Application_Core DeployUtils_Core Qt5::Core)
endif()

# add_custom_target(
#   COMMAND mdtcpbindeps -h
# )

# Export the utils target to a file, so it can be imported by another build
if(NOT CMAKE_CROSSCOMPILING)
  export(
    TARGETS mdtcpbindeps
    FILE "${CMAKE_BINARY_DIR}/ImportDeployUtils.cmake"
  )
endif()
