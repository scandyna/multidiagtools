/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "LibraryInfoTest.h"
#include "Mdt/LibraryInfo.h"
#include <QString>
#include <QFileInfo>

#include <QDebug>

using Mdt::LibraryInfo;

/*
 * Helpers
 */

QString libraryInfoBinDir()
{
  return QString::fromLocal8Bit(MDT_LIBRARY_INFO_BIN_DIR);
}

QString prefixPath()
{
  return QString::fromLocal8Bit(CMAKE_BINARY_DIR);
}

/*
 * On Windows, CMAKE dos not include the drive letter in the path
 */
void removeDriveLetter(QString & path)
{
  Q_ASSERT(path.length() > 2);

  if(path.at(1) == QChar::fromLatin1(':')){
    path.remove(0, 2);
  }
}

void removeDriveLetter(Mdt::Expected<QString> & path)
{
  Q_ASSERT(path);
  removeDriveLetter(*path);
}

/*
 * Tests
 */

void LibraryInfoTest::pathTest()
{
  Mdt::Expected<QString> path;

  qDebug() << libraryInfoBinDir();
  qDebug() << prefixPath();

  path = LibraryInfo::getInstalledLibrariesPath();
  QVERIFY(path);
  removeDriveLetter(path);
  QCOMPARE(*path, libraryInfoBinDir());

  path = LibraryInfo::getPrefixPath();
  QVERIFY(path);
  removeDriveLetter(path);
  QCOMPARE(*path, prefixPath());

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  LibraryInfoTest test;

  return QTest::qExec(&test, argc, argv);
}

