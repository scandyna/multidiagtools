
# Helper function to install icons.
function(mdt_install_icons)
  # Parse arguments
  set(oneValueArgs THEME IS_KNOWN_THEME)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Check and set our local variables
  if(NOT VAR_THEME)
    message(FATAL_ERROR "mdt_install_icons(): THEME argument is missing.")
  endif()
  set(theme ${VAR_THEME})
  set(is_known_theme ${VAR_IS_KNOWN_THEME})

  # On Unix, avoid installing known theme in /usr/share/icons,
  # because they will conflict with those provided by the distribution
  if(UNIX AND is_known_theme AND (NOT CMAKE_INSTALL_PREFIX OR (${CMAKE_INSTALL_PREFIX} STREQUAL "/usr")))
    message("Icons for theme ${theme} will not be installed to avoid conflicting with system installed ones.")
    return()
  endif()

  install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/icons/${theme}"
    DESTINATION "share/icons"
    PATTERN ".*" EXCLUDE
    PATTERN "~*" EXCLUDE
    PATTERN "*.sh" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
  )

endfunction()


mdt_install_icons(THEME "oxygen" IS_KNOWN_THEME true)
