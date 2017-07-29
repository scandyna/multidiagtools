
# List of source files
set(SOURCE_FILES
    Mdt/DeployUtils/SettingsCommandLineParser.cpp
    Mdt/DeployUtils/LibraryVersion.cpp
    Mdt/DeployUtils/LibraryName.cpp
    Mdt/DeployUtils/LibraryInfo.cpp
    Mdt/DeployUtils/LibraryInfoList.cpp
    Mdt/DeployUtils/Library.cpp
    Mdt/DeployUtils/PathList.cpp
    Mdt/DeployUtils/ToolExecutableWrapper.cpp
    Mdt/DeployUtils/LddWrapper.cpp
    Mdt/DeployUtils/Impl/Ldd/DependenciesParserImpl.cpp
    Mdt/DeployUtils/LddDependenciesParser.cpp
    Mdt/DeployUtils/Platform.cpp
    Mdt/DeployUtils/BinaryDependencies.cpp
    Mdt/DeployUtils/BinaryDependenciesImplementationInterface.cpp
    Mdt/DeployUtils/BinaryDependenciesLdd.cpp
)

find_package(Qt5 COMPONENTS Core)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif()

mdt_add_library(
  NAME DeployUtils_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Application Error PlainText_Core Algorithm Qt5::Core
)
mdt_set_library_description(
  NAME DeployUtils_Core
  DESCRIPTION "ItemModel provides some tools for application deployment."
)
