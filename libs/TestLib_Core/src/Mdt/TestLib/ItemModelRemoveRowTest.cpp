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
#include "ItemModelRemoveRowTest.h"
#include "ItemModel.h"
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QVariantList>

// #include <QDebug>

namespace Mdt{ namespace TestLib{

ItemModelRemoveRowTest::ItemModelRemoveRowTest(QAbstractItemModel* model, QObject* parent)
 : QObject(parent)
{
  Q_ASSERT(model != nullptr);

  runTests(model);
}

void ItemModelRemoveRowTest::runTests(QAbstractItemModel* model)
{
  /*
   * Work only on int values for data
   * (some container are internally not based on QVariant)
   */
  QSignalSpy rowsRemovedSpy(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
  QVERIFY(rowsRemovedSpy.isValid());
  QVariantList arguments;
  /*
   * Fill the model with some data
   * ---
   * |1|
   * ---
   * |2|
   * ---
   * |3|
   * ---
   * |4|
   * ---
   * |5|
   * ---
   */
  QCOMPARE(model->rowCount(), 0);
  QVERIFY(model->insertRows(0, 5));
  QCOMPARE(model->rowCount(), 5);
  QVERIFY(setModelData(model, 0, 0, 1));
  QVERIFY(setModelData(model, 1, 0, 2));
  QVERIFY(setModelData(model, 2, 0, 3));
  QVERIFY(setModelData(model, 3, 0, 4));
  QVERIFY(setModelData(model, 4, 0, 5));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 3, 0), QVariant(4));
  QCOMPARE(getModelData(model, 4, 0), QVariant(5));
  /*
   * Remove 3 and 4
   * ---
   * |1|
   * ---
   * |2|
   * ---
   * |5|
   * ---
   */
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QVERIFY(model->removeRows(2, 2));
  // Check data
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(5));
  // Check signal
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(3)); // last
  rowsRemovedSpy.clear();
  /*
   * Remove last row
   * ---
   * |1|
   * ---
   * |2|
   * ---
   */
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QVERIFY(model->removeRow(2));
  // Check data
  QCOMPARE(model->rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  // Check signal
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  rowsRemovedSpy.clear();
  /*
   * Remove first row
   * ---
   * |2|
   * ---
   */
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QVERIFY(model->removeRow(0));
  // Check data
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  // Check signal
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
  rowsRemovedSpy.clear();
}

}} // namespace Mdt{ namespace TestLib{
