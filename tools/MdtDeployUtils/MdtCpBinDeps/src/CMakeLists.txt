
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
  # Commands to install the program
  if( CMAKE_INSTALL_PREFIX AND NOT ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr") )
    set_target_properties(mdtcpbindeps PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
  endif()
  install(
    TARGETS mdtcpbindeps
    EXPORT mdtcpbindepsTargets
    RUNTIME DESTINATION bin COMPONENT tools
  )
  # Commands to generate Config.cmake file
  install(
    EXPORT mdtcpbindepsTargets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mdtcpbindeps"
#     NAMESPACE ${PROJECT_NAME}::
    COMPONENT tools
  )
  set(config_in_file "${CMAKE_CURRENT_BINARY_DIR}/mdtcpbindepsConfig.cmake.in")
  file(WRITE "${config_in_file}" "@PACKAGE_INIT@\n")
#   set(mdt_dep_targets)
#   mdt_get_library_internal_dependencies(
#     mdt_dep_targets
#     DEP_TARGETS ${link_deps}
#   )
#   if(mdt_dep_targets)
#     file(APPEND "${config_in_file}" "set(@PROJECT_NAME@${target_name}Dependencies \"${mdt_dep_targets}\")\n")
#     file(APPEND "${config_in_file}" "foreach(dependency \${@PROJECT_NAME@${target_name}Dependencies})\n")
#     file(APPEND "${config_in_file}" "  find_package(\n")
#     file(APPEND "${config_in_file}" "    @PROJECT_NAME@\${dependency}\n")
#     file(APPEND "${config_in_file}" "    CONFIG\n")
#     file(APPEND "${config_in_file}" "    PATHS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH\n")
#     file(APPEND "${config_in_file}" "  )\n")
#     file(APPEND "${config_in_file}" "endforeach()\n")
#   endif()
  file(APPEND "${config_in_file}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/mdtcpbindepsTargets.cmake\")\n")
  file(APPEND "${config_in_file}" "check_required_components(mdtcpbindeps)")
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    "${config_in_file}"
    "${CMAKE_CURRENT_BINARY_DIR}/mdtcpbindepsConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
  )
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/mdtcpbindepsConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mdtcpbindeps"
    COMPONENT tools
  )
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
