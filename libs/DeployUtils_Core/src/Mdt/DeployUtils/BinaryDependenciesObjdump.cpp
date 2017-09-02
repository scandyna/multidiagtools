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
#include "LibraryName.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <array>
#include <algorithm>

#include <QDebug>

using namespace Mdt::PlainText;

namespace Mdt{ namespace DeployUtils{

/*! \internal List of library to not deploy
 *
 * A good starting point can be found on Wikipedia:
 * https://en.wikipedia.org/wiki/Microsoft_Windows_library_files
 */
static const std::array<const char * const, 28> LibrayExcludeListWindows =
{
  "Hal", "NTDLL", "KERNEL32", "GDI32", "USER32", "COMCTL32", "WS2_32", "ADVAPI32", "NETAPI32", "SHSCRAP",
  "WINMM", "MSVCRT", "mpr", "ole32", "shell32", "version", "crypt32", "dnsapi", "iphlpapi","opengl32",
  "UxTheme", "dwmapi", "imm32", "oleaut32", "Secur32", "odbc32", "shfolder", "wsock32"
};

/*
 * Libraries I don't really know if they must be excluded:
 *  - eay32 , ssleay32 (OpenSSL)
 */

BinaryDependenciesObjdump::BinaryDependenciesObjdump(QObject* parent)
 : BinaryDependenciesImplementationInterface(parent)
{
}

bool BinaryDependenciesObjdump::findDependencies(const QString& binaryFilePath)
{
  setLibrarySearchPathList();
  qDebug() << "Search libraries in:";
  for(const auto & path : mLibrarySearchPathList){
    qDebug() << " " << path;
  }
  // Find dependencies recursively
  mLibraryTree.clear();
  const auto node = mLibraryTree.setRootBinary(binaryFilePath);
  mAlreadyProcessedLibraries.clear();
  if(!findAndAddDependenciesForNode(binaryFilePath, node)){
    return false;
  }
  // Store dependencies
  qDebug() << "Found deps:";
  const auto libs = mLibraryTree.toFlatList();
  LibraryInfoList libraryInfoList;
  for(const auto lib : libs){
    LibraryInfo libraryInfo;
    QFileInfo fi(lib);
    libraryInfo.setAbsoluteFilePath(fi.absoluteFilePath());
    libraryInfo.setLibraryPlatformName(fi.fileName());
    libraryInfoList.addLibrary(libraryInfo);
    qDebug() << " lib: " << lib;
  }
  setDependencies(libraryInfoList);

  return true;
}

bool BinaryDependenciesObjdump::findDependencies(const LibraryInfoList & libraries)
{
  Q_ASSERT(!libraries.isEmpty());

  setLibrarySearchPathList();
  qDebug() << "Search libraries in:";
  for(const auto & path : mLibrarySearchPathList){
    qDebug() << " " << path;
  }
  // Find dependencies recursively
  mAlreadyProcessedLibraries.clear();
  mLibraryTree.clear();
  const auto node = mLibraryTree.setRootBinary(libraries.at(0).absoluteFilePath());
  for(const auto & library : libraries){
    qDebug() << "Process " << library.libraryName().name();
    Q_ASSERT(!library.absoluteFilePath().isEmpty());
    if(!findAndAddDependenciesForNode(library.absoluteFilePath(), node)){
      return false;
    }
  }
  // Store dependencies
  qDebug() << "Found deps:";
  const auto libs = mLibraryTree.toFlatList();
  LibraryInfoList libraryInfoList;
  for(const auto lib : libs){
    LibraryInfo libraryInfo;
    QFileInfo fi(lib);
    libraryInfo.setAbsoluteFilePath(fi.absoluteFilePath());
    libraryInfo.setLibraryPlatformName(fi.fileName());
    libraryInfoList.addLibrary(libraryInfo);
    qDebug() << " lib: " << lib;
  }
  setDependencies(libraryInfoList);

  return true;

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
//     qDebug() << "  Add to tree: " << QFileInfo(binaryFilePath).fileName();
    mLibraryTree.addLibrary(binaryFilePath, node);
    return true;
  }

//   qDebug() << " Find deps for " << QFileInfo(binaryFilePath).fileName() << " ...";
  
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
    if(!isLibraryInExcludeList(record)){
      Library library;
      if( !library.findLibrary(record.data(0), mLibrarySearchPathList, Library::ExcludeSystemPaths) ){
        const QString msg = tr("Could not find library '%1'.\nSearched in %2")
        .arg(record.data(0)).arg(mLibrarySearchPathList.toStringList().join(", "));
        auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
        setLastError(error);
        return false;
      }
      qDebug() << "  Dep: " << record.data(0);
      mAlreadyProcessedLibraries.append(binaryFilePath);
      const auto libraryFilePath = library.libraryInfo().absoluteFilePath();
      const auto libraryNode = mLibraryTree.addLibrary( libraryFilePath, node );
      if(!findAndAddDependenciesForNode( libraryFilePath, libraryNode )){
//         qDebug() << "Fail for " << binaryFilePath;
        return false;
      }
    }
//     Library library;
//     /// \todo Once exclude list(s) done, change here
//     if( library.findLibrary(record.data(0), mLibrarySearchPathList, Library::ExcludeSystemPaths) ){
//       qDebug() << "  Dep: " << record.data(0);
// //       qDebug() << "  Path: " << library.libraryInfo().absoluteFilePath();
//       mAlreadyProcessedLibraries.append(binaryFilePath);
//       const auto libraryFilePath = library.libraryInfo().absoluteFilePath();
//       const auto libraryNode = mLibraryTree.addLibrary( libraryFilePath, node );
//       if(!findAndAddDependenciesForNode( libraryFilePath, libraryNode )){
// //         qDebug() << "Fail for " << binaryFilePath;
//         return false;
//       }
//     }else{
//       qDebug() << "  NOT found: " << record.data(0);
//     }
  }

  return true;
}

bool BinaryDependenciesObjdump::isLibraryInExcludeList(const StringRecord & record)
{
  Q_ASSERT(record.columnCount() > 0);

  const auto cmp = [record](const char * const excludeName){
    LibraryName libName(record.data(0));
    return ( QString::compare( libName.name(), QLatin1String(excludeName), Qt::CaseInsensitive ) == 0);
  };
  const auto it = std::find_if( LibrayExcludeListWindows.cbegin(), LibrayExcludeListWindows.cend(), cmp );
  return (it != LibrayExcludeListWindows.cend());
}

void BinaryDependenciesObjdump::setLibrarySearchPathList()
{
  mLibrarySearchPathList = librarySearchFirstPathList();
#ifdef Q_OS_WIN
  mLibrarySearchPathList.appendPathList( PathList::getSystemLibraryPathList() );
#else
  /*
   * It seems we are cross-compiling
   */
  mLibrarySearchPathList.appendPathList( PathList::getSystemLibraryKnownPathListWindows() );
#endif // #ifdef Q_OS_WIN
//   QFileInfo fi(binaryFilePath);
//   if(!fi.exists()){
//     const auto msg = tr("File '%1' does not exist.").arg(binaryFilePath);
//     auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
//     setLastError(error);
//     return false;
//   }
//   mLibrarySearchPathList.prependPathList( librarySearchFirstPathList() );
//   mLibrarySearchPathList.prependPath( fi.absoluteDir().absolutePath() );
// 
//   return true;
}

}} // namespace Mdt{ namespace DeployUtils{
