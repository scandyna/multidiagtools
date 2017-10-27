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
#ifndef FILE_COPIER_TEST_H
#define FILE_COPIER_TEST_H

#include "TestBase.h"
#include "Mdt/DeployUtils/LibraryInfoList.h"
#include "Mdt/DeployUtils/QtPluginInfoList.h"
#include <QStringList>
#include <QString>
#include <utility>
#include <vector>

class FileCopierTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void createDirectoryTest();
  void copyLibrariesTest();
  void copyQtPluginsTest();

 private:

  static Mdt::DeployUtils::LibraryInfoList createLibraryInfoList(const QStringList & libNameList, const QString & pathPrefix);

  using QtPluginNameAndDir = std::pair<const QString, const QString>;
  static Mdt::DeployUtils::QtPluginInfoList createQtPluginInfoList(const std::vector<QtPluginNameAndDir> & qtPluginsDef, const QString & pathPrefix);
};

#endif // #ifndef FILE_COPIER_TEST_H
