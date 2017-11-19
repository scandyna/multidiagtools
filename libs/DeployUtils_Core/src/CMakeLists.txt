
# List of source files
set(SOURCE_FILES
    Mdt/DeployUtils/ConsoleStream.cpp
    Mdt/DeployUtils/Console.cpp
    Mdt/DeployUtils/LibraryVersion.cpp
    Mdt/DeployUtils/LibraryName.cpp
    Mdt/DeployUtils/LibraryInfo.cpp
    Mdt/DeployUtils/LibraryInfoList.cpp
    Mdt/DeployUtils/Library.cpp
    Mdt/DeployUtils/Impl/LibraryTree/Vertex.cpp
    Mdt/DeployUtils/Impl/LibraryTree/Graph.cpp
    Mdt/DeployUtils/Impl/LibraryTree/LabeledGraph.cpp
    Mdt/DeployUtils/LibraryTreeNode.cpp
    Mdt/DeployUtils/LibraryTree.cpp
    Mdt/DeployUtils/PathList.cpp
    Mdt/DeployUtils/SearchPathList.cpp
    Mdt/DeployUtils/ToolExecutableWrapper.cpp
    Mdt/DeployUtils/LddWrapper.cpp
    Mdt/DeployUtils/ObjdumpWrapper.cpp
    Mdt/DeployUtils/Impl/Ldd/DependenciesParserImpl.cpp
    Mdt/DeployUtils/LddDependenciesParser.cpp
    Mdt/DeployUtils/Impl/Objdump/DependenciesParserImplWindows.cpp
    Mdt/DeployUtils/ObjdumpDependenciesParser.cpp
    Mdt/DeployUtils/Impl/Objdump/BinaryFormatParserImpl.cpp
    Mdt/DeployUtils/ObjdumpBinaryFormatParser.cpp
    Mdt/DeployUtils/BinaryFormat.cpp
    Mdt/DeployUtils/Platform.cpp
    Mdt/DeployUtils/BinaryDependencies.cpp
    Mdt/DeployUtils/BinaryDependenciesImplementationInterface.cpp
    Mdt/DeployUtils/BinaryDependenciesLdd.cpp
    Mdt/DeployUtils/BinaryDependenciesObjdump.cpp
    Mdt/DeployUtils/FileCopier.cpp
    Mdt/DeployUtils/QtPluginInfo.cpp
    Mdt/DeployUtils/QtPluginInfoList.cpp
    Mdt/DeployUtils/QtLibrary.cpp
    Mdt/DeployUtils/QtModule.cpp
    Mdt/DeployUtils/QtModuleList.cpp
    Mdt/DeployUtils/RPathInfo.cpp
    Mdt/DeployUtils/RPathInfoList.cpp
    Mdt/DeployUtils/PatchelfWrapper.cpp
    Mdt/DeployUtils/RPath.cpp
    Mdt/DeployUtils/QmFileName.cpp
    Mdt/DeployUtils/TranslationInfo.cpp
    Mdt/DeployUtils/TranslationInfoList.cpp
    Mdt/DeployUtils/FindTranslation.cpp
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
  LINK_DEPENDENCIES Error_Core PlainText_Core Algorithm Qt5::Core
)
mdt_set_library_description(
  NAME DeployUtils_Core
  DESCRIPTION "ItemModel provides some tools for application deployment."
)
