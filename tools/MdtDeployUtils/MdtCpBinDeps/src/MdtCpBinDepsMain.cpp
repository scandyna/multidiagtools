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
#include "Mdt/DeployUtils/FileCopier.h"
#include "Mdt/DeployUtils/Console.h"
#include <QCoreApplication>
#include <QString>

// #include <QDebug>

using namespace Mdt::DeployUtils;

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

  LibraryInfoList dependencies;
  const auto pathPrefixList = parser.searchFirstPathPrefixList();
  /*
   * Find dependencies for given binary file
   */
  Console::info(1) << "Searching dependencies";
  BinaryDependencies binDeps;
  if(!binDeps.findDependencies(parser.binaryFilePathList(), pathPrefixList)){
    Console::error() << "Searching dependencies failed: " << binDeps.lastError();
    return 1;
  }
  dependencies = binDeps.dependencies();
  /*
   * Find dependencies of dependent Qt libraries plugins
   */
  Console::info(1) << "Searching dependencies for Qt plugins";
  QtLibrary qtLibrary;
  const auto qtLibraries = qtLibrary.getQtLibraries(binDeps.dependencies());
  const auto qtPlugins = qtLibrary.findLibrariesPlugins(qtLibraries, pathPrefixList);
  if(!binDeps.findDependencies(qtPlugins, pathPrefixList)){
    Console::error() << "Searching dependencies for Qt plugins failed: " << binDeps.lastError();
    return 1;
  }
  dependencies.addLibraries(binDeps.dependencies());

  Console::info(1) << "Copy dependencies to " << parser.destinationDirectoryPath();
  FileCopier cp;
  if(!cp.copyLibraries(dependencies, parser.destinationDirectoryPath())){
    Console::error() << "Copy dependencies failed: " << cp.lastError();
    return 1;
  }
  Console::info(1) << "Copy of dependencies successfully done";

  return 0;
}
