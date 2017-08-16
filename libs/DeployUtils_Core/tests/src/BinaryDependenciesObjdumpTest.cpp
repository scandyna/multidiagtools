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
#include "BinaryDependenciesObjdumpTest.h"
#include "Mdt/DeployUtils/LibraryInfo.h"
#include "Mdt/DeployUtils/BinaryDependenciesObjdump.h"
#include "Mdt/PlainText/TestUtils.h"
#include <QChar>
#include <QByteArray>
#include <QString>
#include <algorithm>

#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void BinaryDependenciesObjdumpTest::initTestCase()
{
}

void BinaryDependenciesObjdumpTest::cleanupTestCase()
{
}


void BinaryDependenciesObjdumpTest::sandbox()
{
  BinaryDependenciesObjdump bd;
  
//   QVERIFY(bd.findDependencies( QCoreApplication::applicationFilePath() ));
  
//   qDebug() << "Dir: " << QCoreApplication::applicationDirPath();
  PathList searchPathList{
    "/home/philippe/opt/build/cross/mxe/usr/i686-w64-mingw32.shared.posix/bin",
    "/home/philippe/opt/build/cross/mxe/usr/i686-w64-mingw32.shared.posix/qt5/bin/"/*,
    "/home/philippe/.wine/drive_c/windows/syswow64/"*/
  };
  bd.setLibrarySearchFirstPathList(searchPathList);
//   QVERIFY(bd.findDependencies("/home/philippe/programmation/multiDiagTools/build/cross/win32/release/bin/clienteditor.exe"));
  bd.findDependencies("/home/philippe/programmation/multiDiagTools/build/cross/win32/release/bin/clienteditor.exe");
  
  qDebug() << "Searched in:";
  const auto paths = bd.librarySearchPathList();
  for(const auto & path : paths){
    qDebug() << " " << path;
  }
}

void BinaryDependenciesObjdumpTest::searchPathListTest()
{
  /*
   * Simply check that directory of the serached binary is in the list,
   * and that at least a other path exists (in all cases, some system paths are required)
   * Also check that setting a library prefix is taken in account.
   */
  BinaryDependenciesObjdump bd;
  bd.setLibrarySearchFirstPathList(PathList{"/some/prefix"});
  bd.findDependencies( QCoreApplication::applicationFilePath() );
  
  qDebug() << "Paths: " << bd.librarySearchPathList().toStringList();
  
  QVERIFY( bd.librarySearchPathList().toStringList().size() >= 2 );
  QCOMPARE( bd.librarySearchPathList().toStringList().at(0), QCoreApplication::applicationDirPath() );
  QCOMPARE( bd.librarySearchPathList().toStringList().at(1), QString("/some/prefix") );
}

/*
 * Tests
 */



/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BinaryDependenciesObjdumpTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
