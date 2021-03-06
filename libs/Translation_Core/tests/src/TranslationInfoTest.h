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
#ifndef TRANSLATION_INFO_TEST_H
#define TRANSLATION_INFO_TEST_H

#include "TestBase.h"

class TranslationInfoTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void infoFromQmFilePathTest();
  void compareInfoTest();
  void compareInfoTest_data();

  void addTranslationTest();
  void infoListFromQmFilePathListTest();

  void listToLanguageCodeTest();
  void listToLanguageCodeTest_data();
  void usedFileSuffixesInListTest();
  void usedFileSuffixesInListTest_data();
  void getTranslationsForLanguageCodeTest();
  void getTranslationsForLanguageCodeTest_data();
  void getTranslationsForFileSuffixesTest();
  void getTranslationsForFileSuffixesTest_data();

};

#endif // #ifndef TRANSLATION_INFO_TEST_H
