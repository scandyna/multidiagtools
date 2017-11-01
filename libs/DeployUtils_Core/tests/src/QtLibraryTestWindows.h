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
#ifndef QT_LIBRARY_TEST_WINDOWS_H
#define QT_LIBRARY_TEST_WINDOWS_H

#include "TestBase.h"
#include "Mdt/DeployUtils/LibraryInfoList.h"
#include <QString>

class QtLibraryTestWindows : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void findPluginsRootTest();
  void findPluginsRootTest_data();

  void findPluginsRootFromSysTest();

  void findPluginsDependenciesTest();

 private:

  static bool containsLibrary(const Mdt::DeployUtils::LibraryInfoList & libraries, const QString & libraryFullName);
};

#endif // #ifndef QT_LIBRARY_TEST_WINDOWS_H
