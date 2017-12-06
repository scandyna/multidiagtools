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
#ifndef FIND_TRANSLATION_TEST_H
#define FIND_TRANSLATION_TEST_H

#include "TestBase.h"

class FindTranslationTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void isTranslationDirectoryTest();

  void findTranslationsRootTest();
  void findTranslationsRootTest_data();

  void findTranslationsTest();
  void findTranslationsTest_data();

 private:

  static bool createQmFiles(const QString & directoryPath, const QString & subDirectory, const QStringList & baseNames, const QStringList & languageSuffixes);
  static bool createQmFiles(const QTemporaryDir & directory, const QString & subDirectory, const QStringList & baseNames, const QStringList & languageSuffixes);
};

#endif // #ifndef FIND_TRANSLATION_TEST_H
