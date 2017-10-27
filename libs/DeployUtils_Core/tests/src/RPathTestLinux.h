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
#ifndef RPATH_TEST_LINUX_H
#define RPATH_TEST_LINUX_H

#include "TestBase.h"
#include <QString>
#include <QTemporaryDir>

class RPathTestLinux : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void patchelfWrapperTest();
  void rpathTest();
  void prependPathForBinInDirTest();

 private:

  static QString copyCurrentExecutable();
  static QString copyCurrentExecutableToDirectory(const QTemporaryDir & dir);
};

#endif // #ifndef RPATH_TEST_LINUX_H
