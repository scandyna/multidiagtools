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
#include "Mdt/Sql/LibraryInfo.h"
#include <QString>
#include <QDir>

using Mdt::Sql::LibraryInfo;

/*
 * Helpers
 */

bool directoryIsValid(const QString & path)
{
  Q_ASSERT(!path.trimmed().isEmpty());

  QDir dir(path);

  return dir.exists();
}

/*
 * Tests
 */

void LibraryInfoTest::pluginDirTest()
{
  auto path = LibraryInfo::getPluginsPath();
  QVERIFY(path);
  QVERIFY(directoryIsValid(*path));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
//   Mdt::CoreApplication app(argc, argv);
  QCoreApplication app(argc, argv);
  LibraryInfoTest test;

  return QTest::qExec(&test, argc, argv);
}
