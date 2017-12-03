
# Set installation dir for the Mdt CMake modules,
# as well as the Config.cmake file.
# (See find_package() documentation about that)
if(UNIX)
  if(APPLE)
    set(MDT_CMAKE_MODULES_INSTALL_DIR "Resources/CMake")
  else()
    set(MDT_CMAKE_MODULES_INSTALL_DIR "share/cmake/MdtCMakeModules")
  endif()
elseif(WIN32)
  set(MDT_CMAKE_MODULES_INSTALL_DIR "cmake")
else()
  message(FATAL_ERROR "Could not determine install dir for MdtCMakeModules")
endif()

# Rules to install the modules
install(
  FILES
    "modules/MdtGenerateEnvScript.cmake"
    "modules/MdtDependenciesUtils.cmake"
    "modules/MdtAddTranslations.cmake"
    "modules/MdtDeployUtils.cmake"
    "modules/MdtInstallCopyTargetsDependenciesScript.cmake.in"
    "modules/MdtInstallDirs.cmake"
    "modules/MdtQtPath.cmake"
  DESTINATION "${MDT_CMAKE_MODULES_INSTALL_DIR}/modules"
  COMPONENT "${PROJECT_NAME}-cmake"
)

# Config.cmake file

include(CMakePackageConfigHelpers)
configure_package_config_file(
  MdtCMakeModulesConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/MdtCMakeModulesConfig.cmake
  INSTALL_DESTINATION "${MDT_CMAKE_MODULES_INSTALL_DIR}"
  PATH_VARS MDT_CMAKE_MODULES_INSTALL_DIR
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MdtCMakeModulesConfig.cmake"
#   "${CMAKE_CURRENT_BINARY_DIR}/MdtCMakeModulesVersion.cmake"
  DESTINATION "${MDT_CMAKE_MODULES_INSTALL_DIR}"
  COMPONENT "${PROJECT_NAME}-cmake"
)
