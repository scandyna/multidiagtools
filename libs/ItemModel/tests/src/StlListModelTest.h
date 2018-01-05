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
#ifndef STL_LIST_MODEL_TEST_H
#define STL_LIST_MODEL_TEST_H

#include "TestBase.h"

class StlListModelTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void stdVectorRoTest();
  void variantListRoTest();
  void roSignalTest();
  void roQtModelTest();

  void stdVectorRwTest();
  void variantListRwTest();
  void setDataSignalTest();
  void rwQtModelTest();

  void insertRowsTest();
  void prependAppendRowTest();

  void removeRowsTest();
  void removeFirstLastRowTest();

  void resizableQtModelTest();
};

#endif // #ifndef STL_LIST_MODEL_TEST_H
