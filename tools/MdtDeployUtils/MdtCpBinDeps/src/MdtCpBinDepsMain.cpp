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
#include <QCoreApplication>
#include <QString>

#include <QDebug>

using namespace Mdt::DeployUtils;

MdtCpBinDepsMain::MdtCpBinDepsMain(QObject* parent)
 : AbstractConsoleApplicationMainFunction(parent)
{
  QCoreApplication::setApplicationName("mdtcpbindeps");
//   QCoreApplication::setApplicationVersion("0.0.1");
}

int MdtCpBinDepsMain::runMain()
{
//   QString binaryFilePath;
//   QString destinationDirectoryPath;
//   {
  CommandLineParser parser;
  if(!parser.process()){
    return 1;
  }
//     binaryFilePath = parser.binaryFilePath();
//     destinationDirectoryPath = parser.destinationDirectoryPath();
//   }
  qDebug() << "Main: file: " << parser.binaryFilePath() << " , dest: " << parser.destinationDirectoryPath();

//   SearchPathList searchFirstPathList;
//   searchFirstPathList.setPathPrefixList(parser.librarySearchFirstPathList());
//   searchFirstPathList.setPathSuffixList(parser.librarySearchFirstPathSuffixList()); /// \todo Or hard coded ?

  LibraryInfoList dependencies;
  const auto pathPrefixList = parser.librarySearchFirstPathList();
  /*
   * Find dependencies for given binary file
   */
  BinaryDependencies binDeps;
//   binDeps.setLibrarySearchFirstPathList(parser.librarySearchFirstPathList());
///   binDeps.setLibrarySearchFirstPathSuffixList(parser.librarySearchFirstPathSuffixList());
  if(!binDeps.findDependencies(parser.binaryFilePath(), pathPrefixList)){
    qDebug() << binDeps.lastError().text();
    return 1;
  }
  dependencies = binDeps.dependencies();
  /*
   * Find dependencies of dependent Qt libraries plugins
   */
  QtLibrary qtLibrary;
  const auto qtLibraries = qtLibrary.getQtLibraries(binDeps.dependencies());
  const auto qtPlugins = qtLibrary.findLibrariesPlugins(qtLibraries, pathPrefixList);
  if(!binDeps.findDependencies(qtPlugins, pathPrefixList)){
    qDebug() << binDeps.lastError().text();
    return 1;
  }
  dependencies.addLibraries(binDeps.dependencies());
//   for(const auto & plugin : qtPlugins){
//     if(!binDeps.findDependencies(plugin.absoluteFilePath(), pathPrefixList)){
//       qDebug() << binDeps.lastError().text();
//       return 1;
//     }
//     // LibraryInfoList takes care about duplicates
//     dependencies.addLibraries(binDeps.dependencies());
//   }

  FileCopier cp;
  if(!cp.copyLibraries(dependencies, parser.destinationDirectoryPath())){
    qDebug() << cp.lastError().text();
    return 1;
  }

  qDebug() << "Copy done";

  return 0;
}
