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
#include "MdtCpBinDepsMain.h"
#include "CommandLineParser.h"
#include "Mdt/DeployUtils/SearchPathList.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/QtLibrary.h"
#include "Mdt/DeployUtils/MdtLibrary.h"
#include "Mdt/DeployUtils/FileCopier.h"
#include "Mdt/DeployUtils/Console.h"
#include "Mdt/DeployUtils/BinaryFormat.h"
#include "Mdt/DeployUtils/OperatingSystem.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/Translation/TranslationInfo.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include "Mdt/DeployUtils/FindTranslation.h"
#include "Mdt/DeployUtils/Translation.h"
#include <QCoreApplication>
#include <QString>
#include <QtGlobal>

#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::Translation;

MdtCpBinDepsMain::MdtCpBinDepsMain(QObject* parent)
 : AbstractConsoleApplicationMainFunction(parent)
{
  QCoreApplication::setApplicationName("mdtcpbindeps");
//   QCoreApplication::setApplicationVersion("0.0.1");
}

int MdtCpBinDepsMain::runMain()
{
  CommandLineParser parser;
  if(!parser.process()){
    return 1;
  }
  Console::setLevel(parser.verboseLevel());

  LibraryInfoList dependentQtPlugins;
  const auto pathPrefixList = parser.searchFirstPathPrefixList();
  if(Console::level() >= 2){
    Console::info(2) << "Search first path prefix list:\n " << pathPrefixList.toStringList().join("\n ");
  }
  /*
   * Find dependencies for given binary file
   */
  Console::info(1) << "Searching dependencies";
  BinaryDependencies binDeps;
  if(!binDeps.findDependencies(parser.binaryFilePathList(), pathPrefixList)){
    Console::error() << "Searching dependencies failed: " << binDeps.lastError();
    return 1;
  }
  const auto dependentLibraries = binDeps.dependencies();
  /*
   * Find Qt libraries dependent plugins and their dependencies
   */
  Console::info(1) << "Searching Qt plugins";
  QtLibrary qtLibrary;
  const auto qtLibraries = qtLibrary.getQtLibraries(binDeps.dependencies());
  const auto qtPlugins = qtLibrary.findLibrariesPlugins(qtLibraries, pathPrefixList);
  const auto qtPluginsLibraries = qtPlugins.toLibraryInfoList();
  Console::info(1) << "Searching dependencies for Qt plugins";
  if(!binDeps.findDependencies(qtPluginsLibraries, pathPrefixList)){
    Console::error() << "Searching dependencies for Qt plugins failed: " << binDeps.lastError();
    return 1;
  }
  const auto qtPluginsDependentLibraries = binDeps.dependencies();
  /*
   * Find used translations
   */
  Console::info(1) << "Searching translations for used Qt libraries";
  /// \todo qtModules should also be used to find plugins
  const auto qtModules = qtLibrary.getModules(qtLibraries);
  const auto qtTranslations = findQtTranslations(qtModules, parser.translations(), pathPrefixList);
  if(!qtTranslations){
    Console::error() << "Searching translations for Qt libraries: " << qtTranslations.error();
    return 1;
  }
//   const auto qtTranslations = qtTranslation.findTranslations(qtLibraries, pathPrefixList);
  Console::info(1) << "Searching translations for used Mdt libraries";
  const auto mdtLibraries = MdtLibrary::getMdtLibraries(dependentLibraries);
  for(const auto & library : mdtLibraries){
    qDebug() << "Mdt lib: " << library.libraryName().fullName();
  }
  const auto mdtTranslations = findMdtTranslations(mdtLibraries, parser.translations(), pathPrefixList);
  if(!mdtTranslations){
    Console::error() << "Searching translations for Mdt libraries: " << mdtTranslations.error();
    return 1;
  }
  /*
   * Create single translation files for each language
   */
  Console::info(1) << "Creating translation files";
  TranslationInfoList allTranslations;
  allTranslations.addTranslations(parser.projectQmFiles());
  allTranslations.addTranslations(*qtTranslations);
  allTranslations.addTranslations(*mdtTranslations);
  for(const auto translation : allTranslations){
    qDebug() << "QM: " << translation.fullFileName();
  }
  Translation translation;
  if(!translation.joinTranslations(allTranslations, parser.binaryFilePathList(), parser.translationDestinationPath(), pathPrefixList)){
    Console::error() << "Creating translation files failed: " << translation.lastError();
    return 1;
  }
  /*
   * Copy dependencies
   */
  FileCopier cp;
  Console::info(1) << "Copy dependent libraries to " << parser.libraryDestinationPath();
  if(!cp.copyLibraries(dependentLibraries, parser.libraryDestinationPath())){
    Console::error() << "Copy failed: " << cp.lastError();
    return 1;
  }
  Console::info(1) << "Copy Qt plugins to " << parser.pluginDestinationPath();
  if(!cp.copyPlugins(qtPlugins, parser.pluginDestinationPath())){
    Console::error() << "Copy failed: " << cp.lastError();
    return 1;
  }
  Console::info(1) << "Copy dependencies of Qt plugins to " << parser.libraryDestinationPath();
  if(!cp.copyLibraries(qtPluginsDependentLibraries, parser.libraryDestinationPath())){
    Console::error() << "Copy failed: " << cp.lastError();
    return 1;
  }
  /*
   * On platform that support it, patch RPATH
   * We do runtime detetction to support cross-compilation
   */
  Q_ASSERT(!parser.binaryFilePathList().isEmpty());
  BinaryFormat bfmt;
  if(!bfmt.readFormat( parser.binaryFilePathList().at(0) )){
    Console::error() << "Deducing file format filed: " << bfmt.lastError();
    return 1;
  }
  if(bfmt.operatingSystem() == OperatingSystem::Linux){
    RPath rpath;
    Console::info(1) << "Updating RPATH of libraries";
    if(!rpath.prependPathForBinaries(".", parser.libraryDestinationPath())){
      Console::error() << "Updating RPATH failed: " << rpath.lastError();
      return 1;
    }
  }

  Console::info(1) << "Copy of dependencies successfully done";

  return 0;
}
