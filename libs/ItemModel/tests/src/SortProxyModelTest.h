/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_MODEL_EDITOR_SORT_PROXY_MODEL_TEST_H
#define MDT_MODEL_EDITOR_SORT_PROXY_MODEL_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <QVariant>
#include <Qt>
#include <vector>

class QAbstractItemModel;

class SortProxyModelTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void columnSortOrderTest();
  void columnSortOrderListTest();
  void columnSortOrderListBenchmark();

  void columnSortStringAttributesTest();
  void columnSortStringAttributesListTest();
  void columnSortStringAttributesBenchmark();

  void sortIntTest();
  void sortIntBenchmark();
  void dynamicSortEventSingleColumnTest();
  void dynamicSortEventMultiColumnTest();
  void viewsTest();
//   void sortAlphaTest();
//   void sortAlphaCiTest();
//   void sortAlphNumNaturalTest();
//   void sortAlphaMultiColumnTest();

 private:

  static QVariant getModelData(const QAbstractItemModel & model, int row, int column);
  static bool isModelIntColumnSorted(const QAbstractItemModel & model, int column);
};

#endif // #ifndef MDT_MODEL_EDITOR_SORT_PROXY_MODEL_TEST_H
