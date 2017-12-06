/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "TestBase.h"
#include <QTextStream>
#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::FileSystem;

QStringList TestBase::sortedStringListCs(const QStringList& inList)
{
  auto list = inList;

  list.sort(Qt::CaseSensitive);

  return list;
}

LibraryInfo TestBase::buildLibraryInfo(const QString& directoryPath, const QString& libraryFileName)
{
  LibraryInfo li;

  const auto libraryPath = QDir::cleanPath( directoryPath + "/" + libraryFileName );
  li.setAbsoluteFilePath(libraryPath);
  li.setLibraryPlatformName(libraryFileName);

  return li;
}

LibraryInfoList TestBase::buildLibraryInfoList(const QString& directoryPath, const QStringList& librariesFileNames)
{
  LibraryInfoList lil;

  for(const auto & libraryFileName : librariesFileNames){
    lil.addLibrary( buildLibraryInfo(directoryPath, libraryFileName) );
  }

  return lil;
}

bool TestBase::compareLibraryInfoList(const LibraryInfoList& lil, const LibraryInfoList& expectedLil)
{
  if(lil.count() != expectedLil.count()){
    qDebug() << "Count of libraries is not the same.\n Actual: " << lil.count() << "\n Expected: " << expectedLil.count();
    return false;
  }
  for(int i = 0; i < lil.count(); ++i){
    if(lil.at(i) != expectedLil.at(i)){
      qDebug() << "Libraries differs at index " << i << ".\n Actual name: " << lil.at(i).libraryName().fullName() << "\n Expected name: " << expectedLil.at(i).libraryName().fullName();
      return false;
    }
  }
  return true;
}

bool TestBase::createFile(const QString & filePath)
{
  const QFileInfo fi(filePath);
  auto dir = fi.absoluteDir();

  if(!dir.mkpath(dir.absolutePath())){
    qDebug() << "Unable to create directory " << dir.absolutePath();
    return false;
  }
  QFile file(fi.absoluteFilePath());
  if(!file.open(QFile::WriteOnly)){
    qDebug() << "Unable to create file " << file.fileName() << ": " << file.errorString();
    return false;
  }
  file.close();

  return true;
}

bool TestBase::createFileInDirectory(const QString& directoryPath, const QString& fileName)
{
  return createFile( QDir::cleanPath(directoryPath + "/" + fileName) );
}

bool TestBase::createFileInTemporaryDirectory(const QTemporaryDir & testRootDirectory, const QString & absoluteFilePath)
{
  const QFileInfo fi(testRootDirectory.path() + absoluteFilePath);

  return createFile(fi.absoluteFilePath());
}

bool TestBase::writeTemporaryTextFile(QTemporaryFile & file, const QString& data, const QByteArray& encoding)
{
  if(!file.open()){
    qDebug() << "Could not create a temporary file";
    return false;
  }
  auto *codec = QTextCodec::codecForName(encoding);
  if(codec == nullptr){
    qDebug() << "Could not find a codec for requested encoding: " << encoding;
    return false;
  }
  QTextStream stream(&file);
  stream.setCodec(codec);
  stream << data;
  file.close();

  return true;
}

QString TestBase::pathWithFakeRoot(const QTemporaryDir& root, const QString& path)
{
  return QDir::cleanPath( root.path() + path );
}

void TestBase::prependFakeRootToPath(const QTemporaryDir& root, QString& path)
{
  path = pathWithFakeRoot(root, path);
}

void TestBase::prependFakeRootToPathList(const QTemporaryDir& root, QStringList& pathList)
{
  for(auto it = pathList.begin(); it != pathList.end(); ++it){
    *it = pathWithFakeRoot(root, *it);
  }
}

void TestBase::prependFakeRootToPathList(const QTemporaryDir & root, PathList & pathList)
{
  auto tmp = pathList.toStringList();
  prependFakeRootToPathList(root, tmp);
  pathList = PathList::fromStringList(tmp);
}

bool TestBase::createPathInFakeRoot(const QTemporaryDir& root, const QString& path)
{
  QDir dir;
  if( !dir.mkpath( pathWithFakeRoot(root, path) ) ){
    qWarning() << "Could not create path " << pathWithFakeRoot(root, path);
    return false;
  }
  return true;
}

bool TestBase::createPathListInFakeRoot(const QTemporaryDir& root, const QStringList& pathList)
{
  for(const auto & path : pathList){
    if(!createPathInFakeRoot(root, path)){
      return false;
    }
  }
  return true;
}
