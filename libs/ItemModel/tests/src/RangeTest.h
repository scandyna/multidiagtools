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
#ifndef MDT_ITEM_MODEL_RANGE_TEST_H
#define MDT_ITEM_MODEL_RANGE_TEST_H

#include <QObject>
#include <QtTest/QtTest>

class RangeTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void initialStateTest();
  void setFirstLastTest();
  void setFirstCountTest();

  void setGetBenchmark();

  void rowInitialStateTest();
  void rowSetFirstLastTest();
  void rowSetFirstCountTest();
  void rowSetFirstLastIndexTest();

  void columnInitialStateTest();
  void columnSetFirstLastTest();
  void columnSetFirstCountTest();
  void columnSetFirstLastIndexTest();
};

#endif // #ifndef MDT_ITEM_MODEL_RANGE_TEST_H
