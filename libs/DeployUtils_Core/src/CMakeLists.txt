
# List of source files
set(SOURCE_FILES
    Mdt/DeployUtils/SettingsCommandLineParser.cpp
    Mdt/DeployUtils/LibraryVersion.cpp
    Mdt/DeployUtils/LibraryName.cpp
    Mdt/DeployUtils/LibraryInfo.cpp
    Mdt/DeployUtils/Library.cpp
    Mdt/DeployUtils/PathList.cpp
    Mdt/DeployUtils/ToolExecutableWrapper.cpp
    Mdt/DeployUtils/LddWrapper.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME DeployUtils_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Application Error Qt5::Core
)
mdt_set_library_description(
  NAME DeployUtils_Core
  DESCRIPTION "ItemModel provides some tools for application deployment."
)
