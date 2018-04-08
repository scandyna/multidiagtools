/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "ItemModelTest.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
#include <QStringList>
#include <QStringListModel>

using namespace Mdt::TestLib;

void ItemModelTest::initTestCase()
{
}

void ItemModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemModelTest::getModelFlagsTest()
{
  QStringListModel model(QStringList{"A"});

  auto flags = getModelFlags(&model, 0, 0);
  QVERIFY( flags.testFlag(Qt::ItemIsEnabled) );
  QVERIFY( flags.testFlag(Qt::ItemIsSelectable) );
  QVERIFY( flags.testFlag(Qt::ItemIsEditable) );
  QVERIFY(!flags.testFlag(Qt::ItemIsTristate) );

  flags = getModelFlags(model, 0, 0);
  QVERIFY( flags.testFlag(Qt::ItemIsEnabled) );
  QVERIFY( flags.testFlag(Qt::ItemIsSelectable) );
  QVERIFY( flags.testFlag(Qt::ItemIsEditable) );
  QVERIFY(!flags.testFlag(Qt::ItemIsTristate) );
}

void ItemModelTest::getModelDataTest()
{
  QStringListModel model(QStringList{"A","B","C"});

  QCOMPARE( getModelData(&model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(&model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(&model, 2, 0), QVariant("C") );

  QCOMPARE( getModelData(model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(model, 2, 0), QVariant("C") );
}

void ItemModelTest::setModelDataTest()
{
  QStringListModel model(QStringList{"A","B","C"});

  QCOMPARE( getModelData(model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(model, 2, 0), QVariant("C") );
  QVERIFY( setModelData(&model, 0, 0, 1) );
  QVERIFY( setModelData( model, 1, 0, 2) );
  QVERIFY( setModelData( model, 2, 0, 3) );
  QCOMPARE( getModelData(model, 0, 0), QVariant(1) );
  QCOMPARE( getModelData(model, 1, 0), QVariant(2) );
  QCOMPARE( getModelData(model, 2, 0), QVariant(3) );
}

void ItemModelTest::prependAppendTest()
{
  QStringListModel model;
  QCOMPARE(model.rowCount(), 0);
  /*
   * Append a row (2x)
   * ---
   * |C|
   * ---
   * |D|
   * ---
   */
  QVERIFY(appendRowToModel(&model));
  QCOMPARE(model.rowCount(), 1);
  QVERIFY(setModelData(model, 0, 0, "C"));
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(setModelData(model, 1, 0, "D"));
  QCOMPARE( getModelData(model, 0, 0), QVariant("C") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("D") );
  /*
   * Prepend a row (2x)
   * ---
   * |a|
   * ---
   * |b|
   * ---
   * |C|
   * ---
   * |D|
   * ---
   */
  QVERIFY(prependRowToModel(&model));
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(setModelData(model, 0, 0, "b"));
  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 4);
  QVERIFY(setModelData(model, 0, 0, "a"));
  QCOMPARE( getModelData(model, 0, 0), QVariant("a") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("b") );
  QCOMPARE( getModelData(model, 2, 0), QVariant("C") );
  QCOMPARE( getModelData(model, 3, 0), QVariant("D") );
}

void ItemModelTest::removeFirstLastTest()
{
  QStringListModel model;
  /*
   * Fill the model with some data
   * ---
   * |1|
   * ---
   * |2|
   * ---
   * |3|
   * ---
   */
  QCOMPARE(model.rowCount(), 0);
  QVERIFY(model.insertRows(0, 3));
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(setModelData(model, 0, 0, 1));
  QVERIFY(setModelData(model, 1, 0, 2));
  QVERIFY(setModelData(model, 2, 0, 3));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  /*
   * Remove the first row
   * ---
   * |2|
   * ---
   * |3|
   * ---
   */
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(removeFirstRowFromModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 0), QVariant(3));
  /*
   * Remove the last row
   * ---
   * |2|
   * ---
   */
  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 0);
}

void ItemModelTest::insertRowTest()
{
  QStringListModel model;
  ItemModelInsertRowTest test(&model);
}

void ItemModelTest::removeRowTest()
{
  QStringListModel model;
  ItemModelRemoveRowTest test(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ItemModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
