/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "QtLibrary.h"
#include "SearchPathList.h"
#include "LibraryName.h"
#include "Console.h"
#include <QLatin1String>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <algorithm>
#include <iterator>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

LibraryInfoList QtLibrary::findLibraryPlugins(const LibraryInfo & qtLibrary, const PathList & searchFirstPathPrefixList)
{
  LibraryInfoList plugins;

  PathList pathPrefixList = searchFirstPathPrefixList;
  pathPrefixList.appendPathList( PathList::getSystemLibraryPathList() );
  SearchPathList searchPathList;
  searchPathList.setPathPrefixList(pathPrefixList);
  searchPathList.setPathSuffixList({"qt5/plugins","plugins"});

  const auto pluginDirectories = getPluginsDirectories( module(qtLibrary) );
  const auto pathList = searchPathList.pathList();
  const auto os = LibraryName::operatingSystem(qtLibrary.libraryName().fullName());
  Q_ASSERT(os != OperatingSystem::Unknown);

  Console::info(2) << " searching plugins for library " << qtLibrary.libraryName().name();
  for(const auto & path : pathList){
    Console::info(3) << "  searching in " << path << " ...";
    plugins = findPluginsInDirectories(path, pluginDirectories, os);
    if(!plugins.isEmpty()){
      break;
    }
  }

  return plugins;
}

LibraryInfoList QtLibrary::findLibrariesPlugins(const LibraryInfoList& qtLibraries, const PathList& searchFirstPathPrefixList)
{
  LibraryInfoList plugins;

  for(const auto & library : qtLibraries){
    const auto itemPlugins = findLibraryPlugins(library, searchFirstPathPrefixList);
    plugins.addLibraries(itemPlugins);
  }

  return plugins;
}

LibraryInfoList QtLibrary::getQtLibraries(const LibraryInfoList & libraries)
{
  LibraryInfoList qtLibraries;

  std::copy_if(libraries.cbegin(), libraries.cend(), std::back_inserter(qtLibraries), isQtLibrary);

  return qtLibraries;
}

QtModule QtLibrary::module(const LibraryInfo & qtLibrary)
{
  const QString libraryName = qtLibrary.libraryName().name();

  if(compareLibraries(libraryName, "Qt5Core")){
    return QtModule::Core;
  }
  if(compareLibraries(libraryName, "Qt5Gui")){
    return QtModule::Gui;
  }
  if(compareLibraries(libraryName, "Qt5Multimedia")){
    return QtModule::Multimedia;
  }
  if(compareLibraries(libraryName, "Qt5MultimediaWidgets")){
    return QtModule::MultimediaWidgets;
  }
  if(compareLibraries(libraryName, "Qt5Network")){
    return QtModule::Network;
  }
  if(compareLibraries(libraryName, "Qt5Qml")){
    return QtModule::Qml;
  }
  if(compareLibraries(libraryName, "Qt5Quick")){
    return QtModule::Quick;
  }
  if(compareLibraries(libraryName, "Qt5Sql")){
    return QtModule::Sql;
  }
  if(compareLibraries(libraryName, "Qt5Test")){
    return QtModule::Test;
  }
  if(compareLibraries(libraryName, "Qt5Widgets")){
    return QtModule::Widgets;
  }

  return QtModule::Unknown;
}

QtModuleList QtLibrary::getModules(const LibraryInfoList & qtLibraries)
{
  QtModuleList modules;

  // QtModuleList will take care of duplicates and unknown modules
  for(const auto & library : qtLibraries){
    modules.addModule( module(library) );
  }

  return modules;
}

QStringList QtLibrary::getPluginsDirectories(QtModule module)
{
  QStringList directories;

  /*
   * Note about Qt Platform Abstraction:
   * QPA is not Qt module that produces a binary library.
   * By seeing dependencies, we deduced to which module to attach each QPA directory
   */
  switch(module){
    case QtModule::Unknown:
      break;
    case QtModule::Core:
      // QPA: Qt Core seems appropriate for platforms
      directories = QStringList{"platforms"};
      break;
    case QtModule::Gui:
      // QPA: Qt GUI seems appropriate for platforminputcontexts and platformthemes
      directories = QStringList{"accessiblebridge","imageformats","pictureformats","platforminputcontexts","platformthemes"};
      break;
    case QtModule::Multimedia:
      directories = QStringList{"audio","video/declarativevideobackend","video/bufferpool","playlistformats","resourcepolicy","mediaservice","video/videonode"};
      break;
    case QtModule::MultimediaWidgets:
      break;
    case QtModule::Network:
      directories = QStringList{"bearer"};
      break;
    case QtModule::Positioning:
      directories = QStringList{"position"};
      break;
    case QtModule::PrintSupport:
      directories = QStringList{"printsupport"};
      break;
    case QtModule::Quick:
      directories = QStringList{"scenegraph"};
      break;
    case QtModule::Sensors:
      directories = QStringList{"sensorgestures","sensors"};
      break;
    case QtModule::Sql:
      directories = QStringList{"sqldrivers"};
      break;
    case QtModule::Svg:
      directories = QStringList{"iconengines"};
      break;
    case QtModule::Widgets:
      directories = QStringList{"accessible","styles"};
      break;
    case QtModule::Qml:
    case QtModule::Test:
    case QtModule::DBus:
      break;
  }

  return directories;
}

QStringList QtLibrary::getPluginsDirectories(const QtModuleList & modules)
{
  QStringList directories;

  for(auto module : modules){
    directories.append(getPluginsDirectories(module));
  }

  return directories;
}

bool QtLibrary::isQtLibrary(const LibraryInfo& libraryInfo)
{
  return libraryInfo.libraryName().name().startsWith(QLatin1String("Qt5"), Qt::CaseInsensitive);

//   const QString libraryName = libraryInfo.libraryName().name();
// 
//   return (
//        compareLibraries(libraryName, "Qt5Core")
//     || compareLibraries(libraryName, "Qt5Widgets")
//     || compareLibraries(libraryName, "Qt5Gui")
//     || compareLibraries(libraryName, "Qt5Network")
//     || compareLibraries(libraryName, "Qt5Sql")
//     || compareLibraries(libraryName, "Qt5Xml")
//     || compareLibraries(libraryName, "Qt5Test")
//     || compareLibraries(libraryName, "Qt5CLucene")
//     || compareLibraries(libraryName, "Qt5Concurrent")
//     || compareLibraries(libraryName, "Qt5Contacts")
//     || compareLibraries(libraryName, "Qt5DBus")
//     || compareLibraries(libraryName, "Qt5DesignerComponents")
//     || compareLibraries(libraryName, "Qt5Designer")
//     || compareLibraries(libraryName, "Qt5EglDeviceIntegration")
//     || compareLibraries(libraryName, "Qt5Help")
//     || compareLibraries(libraryName, "Qt5Location")
//     || compareLibraries(libraryName, "Qt5Multimedia")
//     || compareLibraries(libraryName, "Qt5MultimediaWidgets")
//     || compareLibraries(libraryName, "Qt5MultimediaQuick_p")
//     || compareLibraries(libraryName, "Qt5OpenGL")
//     || compareLibraries(libraryName, "Qt5Organizer")
//     || compareLibraries(libraryName, "Qt5Positioning")
//     || compareLibraries(libraryName, "Qt5PrintSupport")
//     || compareLibraries(libraryName, "Qt5Qml")
//     || compareLibraries(libraryName, "Qt5Quick")
//     || compareLibraries(libraryName, "Qt5QuickWidgets")
//     || compareLibraries(libraryName, "Qt5QuickParticles")
//     || compareLibraries(libraryName, "Qt5QuickTest")
//     || compareLibraries(libraryName, "Qt5Sensors")
//     || compareLibraries(libraryName, "Qt5SerialPort")
//     || compareLibraries(libraryName, "Qt5Svg")
//     || compareLibraries(libraryName, "Qt5Versit")
//     || compareLibraries(libraryName, "Qt5VersitOrganizer")
//     || compareLibraries(libraryName, "Qt5")
//     || compareLibraries(libraryName, "Qt5")
//     || compareLibraries(libraryName, "Qt5")
//     || compareLibraries(libraryName, "Qt5")
//   );
}

bool QtLibrary::compareLibraries(const QString & a, const char*const b)
{
  return ( QString::compare(a, QLatin1String(b), Qt::CaseInsensitive) == 0 );
}

LibraryInfoList QtLibrary::findPluginsInDirectories(const QString & pathPrefix, const QStringList& directories, OperatingSystem os)
{
  LibraryInfoList plugins;

  for(const auto & directory : directories){
    QDir dir( QDir::cleanPath(pathPrefix + "/" + directory) );
    if(dir.exists()){
      const auto fiList = dir.entryInfoList(QDir::Files);
      for(const auto & fi : fiList){
        // When do cross compilation, don't bring wrong libraries to the list
        if(LibraryName::operatingSystem(fi.fileName()) == os){
          Console::info(4) << "   found " << fi.fileName();
          LibraryInfo li;
          li.setLibraryPlatformName(fi.fileName());
          li.setAbsoluteFilePath(fi.absoluteFilePath());
          plugins.addLibrary(li);
        }
      }
    }
  }

  return plugins;
}

}} // namespace Mdt{ namespace DeployUtils{
