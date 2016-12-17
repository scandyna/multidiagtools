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
class QSortFilterProxyModel;

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
  void sortIntBenchmark_data();

  void sortRoleTest();

  void sortStringCsTest();
  void sortStringNumericModeTest();
  void sortStringIntMixedTest();

  void sortStringNonLocalAwareBenchmark();
  void sortStringNonLocalAwareBenchmark_data();
  void sortStringLocalAwareBenchmark();
  void sortStringLocalAwareBenchmark_data();

  void sortSetModelTest();
  void sortColumnCountChangeTest();

  void sortSignalTest();
  void sortSetterEventTest();
  void dynamicSortTest();

  // Should mybe be implemented in SQL ItemModel library ?
  void fetchTest();

 private:

  static QVariant getModelData(const QAbstractItemModel & model, int row, int column);
  static bool isModelColumnSortedNumeric(const QAbstractItemModel & model, int column);
  static bool isModelColumnSortedAscii(const QAbstractItemModel & model, int column, Qt::CaseSensitivity caseSensitiviy);
  static void displayModels(QAbstractItemModel *sourceModel, QSortFilterProxyModel *proxyModel);
};

#endif // #ifndef MDT_MODEL_EDITOR_SORT_PROXY_MODEL_TEST_H
