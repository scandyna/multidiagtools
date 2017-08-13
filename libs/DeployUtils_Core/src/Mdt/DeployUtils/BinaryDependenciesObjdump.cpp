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
#include "BinaryDependenciesObjdump.h"
#include "ObjdumpWrapper.h"
#include "ObjdumpDependenciesParser.h"
#include "Library.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QFileInfo>
#include <QDir>
#include <QString>

#include <QDebug>

using namespace Mdt::PlainText;

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesObjdump::BinaryDependenciesObjdump(QObject* parent)
 : BinaryDependenciesImplementationInterface(parent)
{
}

bool BinaryDependenciesObjdump::findDependencies(const QString& binaryFilePath)
{
  if(!setLibrarySearchPathList(binaryFilePath)){
    return false;
  }
  qDebug() << "Search libraries in:";
  for(const auto & path : mLibrarySearchPathList){
    qDebug() << " " << path;
  }

  const auto node = mLibraryTree.setRootBinary(binaryFilePath);
  mAlreadyProcessedLibraries.clear();
  if(!findAndAddDependenciesForNode(binaryFilePath, node)){
    return false;
  }

  const auto libs = mLibraryTree.toFlatList();
  qDebug() << "Found deps:";
  for(const auto lib : libs){
    qDebug() << " lib: " << lib;
  }
  
  return false;
}

bool BinaryDependenciesObjdump::findAndAddDependenciesForNode(const QString& binaryFilePath, LibraryTreeNode node)
{
  /*
   * If a library allready exists in the tree,
   * we just add it as child of the node.
   * Else, we search the dependencies.
   */
  
//   qDebug() << "Process " << QFileInfo(binaryFilePath).fileName();
  
  if(mAlreadyProcessedLibraries.contains(binaryFilePath)){
    mLibraryTree.addLibrary(binaryFilePath, node);
    return true;
  }

  qDebug() << " Find deps for " << QFileInfo(binaryFilePath).fileName() << " ...";
  
  /// \todo If binaryFilePath allready exists in the tree,
  
  ObjdumpWrapper objdump;
  if(!objdump.execFindDependencies(binaryFilePath)){
    setLastError(objdump.lastError());
    return false;
  }

  ObjdumpDependenciesParser parser;
  if(!parser.parse( objdump.readAllStandardOutputString() )){
    const QString msg = tr("Parsing objdump output for file '%1' failed.").arg(binaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  const auto rawDependenciesRecordList = parser.rawDependencies();
  for(const auto & record : rawDependenciesRecordList){
    Q_ASSERT(record.columnCount() == 1);
    Library library;
    /// \todo Once exclude list(s) done, change here
    if( library.findLibrary(record.data(0), mLibrarySearchPathList, Library::ExcludeSystemPaths) ){
      qDebug() << "  Dep: " << record.data(0);
//       qDebug() << "  Path: " << library.libraryInfo().absoluteFilePath();
      mAlreadyProcessedLibraries.append(binaryFilePath);
      const auto libraryFilePath = library.libraryInfo().absoluteFilePath();
      const auto libraryNode = mLibraryTree.addLibrary( libraryFilePath, node );
      if(!findAndAddDependenciesForNode( libraryFilePath, libraryNode )){
//         qDebug() << "Fail for " << binaryFilePath;
        return false;
      }
    }else{
      qDebug() << "  NOT found: " << record.data(0);
    }
  }

  return true;
}

bool BinaryDependenciesObjdump::setLibrarySearchPathList(const QString& binaryFilePath)
{
  mLibrarySearchPathList.clear();
#ifdef Q_OS_WIN
  mLibrarySearchPathList.appendPathList( PathList::getSystemLibraryPathList() );
#else
  /*
   * It seems we are cross-compiling
   */
  mLibrarySearchPathList.appendPathList( PathList::getSystemLibraryKnownPathListWindows() );
#endif // #ifdef Q_OS_WIN
  QFileInfo fi(binaryFilePath);
  if(!fi.exists()){
    const auto msg = tr("File '%1' does not exist.").arg(binaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  mLibrarySearchPathList.prependPathList( librarySearchFirstPathList() );
  mLibrarySearchPathList.prependPath( fi.absoluteDir().absolutePath() );

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
