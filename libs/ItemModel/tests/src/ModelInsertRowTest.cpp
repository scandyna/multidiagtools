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
#include "ModelInsertRowTest.h"
#include "TestBase.h"
#include <QSignalSpy>
#include <QVariantList>

// #include <QDebug>

/*
 * Helpers
 */

bool isEditableAt(QAbstractItemModel * model, int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < model->rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model->columnCount());

  return ( getModelFlags(model, row, column) & Qt::ItemIsEditable );
}

/*
 * Tests
 */

ModelInsertRowTest::ModelInsertRowTest(QAbstractItemModel* model, QObject* parent)
 : QObject(parent)
{
  Q_ASSERT(model != nullptr);

  runTests(model);
}

void ModelInsertRowTest::runTests(QAbstractItemModel* model)
{
  QSignalSpy rowsInsertedSpy(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)));
  QVERIFY(rowsInsertedSpy.isValid());
  QVariantList arguments;
  /*
   * Work only on int values for data
   * (some container are internally not based on QVariant)
   */
  QCOMPARE(model->rowCount(), 0);
  /*
   * Append a row
   * ---
   * |1|
   * ---
   */
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model->rowCount(), 1);
  QVERIFY(model->columnCount() > 0);
  QVERIFY(getModelData(model, 0, 0).isNull());
  // Check that inserted row can be used
  if(isEditableAt(model, 0, 0)){
    QVERIFY(setModelData(model, 0, 0, 1));
    QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  }
  // Check signal
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
  rowsInsertedSpy.clear();
  /*
   * Prepend a row
   * ----
   * |-1|
   * ---
   * | 1|
   * ----
   */
  QCOMPARE(rowsInsertedSpy.count(), 0);
  prependRowToModel(model);
  QCOMPARE(model->rowCount(), 2);
  QVERIFY(getModelData(model, 0, 0).isNull());
  // Check that row was inserted at correct place
  if(isEditableAt(model, 1, 0)){  // Was previously row 0
    QCOMPARE(getModelData(model, 1, 0), QVariant(1));
  }
  // Check that inserted row can be used
  if(isEditableAt(model, 0, 0)){
    QVERIFY(setModelData(model, 0, 0, -1));
    QCOMPARE(getModelData(model, 0, 0), QVariant(-1));
  }
  // Check signal
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
  rowsInsertedSpy.clear();
  /*
   * Insert 2 rows before 1
   * Before:
   *  ----
   *  |-1|
   * >----
   *  | 1|
   *  ---
   * After:
   * ----
   * |-1|
   * ----
   * | 2|
   * ----
   * | 3|
   * ----
   * | 1|
   * ----
   */
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QVERIFY(model->insertRows(1, 2));
  QCOMPARE(model->rowCount(), 4);
  QVERIFY(getModelData(model, 1, 0).isNull());
  QVERIFY(getModelData(model, 2, 0).isNull());
  // Check that rows where inserted at correctplace
  if(isEditableAt(model, 0, 0)){
    QCOMPARE(getModelData(model, 0, 0), QVariant(-1));
  }
  if(isEditableAt(model, 3, 0)){
    QCOMPARE(getModelData(model, 3, 0), QVariant(1));
  }
  // Check that inserted row can be used
  if(isEditableAt(model, 1, 0)){
    QVERIFY(setModelData(model, 1, 0, 2));
    QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  }
  if(isEditableAt(model, 2, 0)){
    QVERIFY(setModelData(model, 2, 0, 3));
    QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  }
  // Check that correct rows have been updated
  if(isEditableAt(model, 0, 0)){
    QCOMPARE(getModelData(model, 0, 0), QVariant(-1));
  }
  if(isEditableAt(model, 1, 0)){
    QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  }
  if(isEditableAt(model, 2, 0)){
    QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  }
  if(isEditableAt(model, 3, 0)){
    QCOMPARE(getModelData(model, 3, 0), QVariant(1));
  }
  // Check signal
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  rowsInsertedSpy.clear();
}
