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
#include "RemoveRowsTest.h"
#include "Mdt/ItemModel/RemoveRows.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QStringList>
#include <QStringListModel>

using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;

/*
 * Tests
 */

void RemoveRowsTest::removeRowsRangeTest()
{
  QStringListModel model(QStringList{"A","B","C","D"});
  RowRange r;

//   QVERIFY( removeRows(&model, r) );
//   QCOMPARE(model.stringList(), QStringList({"A","B","C","D"}));

  r.setFirstRow(1);
  r.setLastRow(2);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList({"A","D"}));

  r.setFirstRow(1);
  r.setLastRow(1);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList({"A"}));

  r.setFirstRow(0);
  r.setLastRow(0);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionTest()
{
  QStringListModel model(QStringList{"A","B","C","D","E","F"});
  RowSelection s;
  RowRange r;

  s.clear();
  r.setFirstRow(4);
  r.setLastRow(5);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList({"A","B","C","D"}));

  s.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  s.appendRange(r);
  r.setFirstRow(3);
  r.setLastRow(3);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList({"C"}));

  s.clear();
  r.setFirstRow(0);
  r.setLastRow(0);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionModelTest()
{
  QStringListModel model(QStringList{"A","B","C","D","E"});
  QItemSelectionModel selectionModel(&model);

  /*
   * A empty selection must be accepted,
   * do nothing and return true.
   */
  selectionModel.clear();
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"A","B","C","D","E"}));

  /*
   * ---
   * |S|
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"B","C","D","E"}));
  /*
   * ---
   * | |
   * ---
   * |S|
   * ---
   * | |
   * ---
   * |S|
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(3, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"B","D"}));
  /*
   * ---
   * |S|
   * ---
   * |S|
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionModelTableTest()
{
  VariantTableModel model;
  model.resize(3, 2);
  QItemSelectionModel selectionModel(&model);

  setModelData(model, 0, 0, "A0");
  setModelData(model, 0, 1, "B0");
  setModelData(model, 1, 0, "A1");
  setModelData(model, 1, 1, "B1");
  setModelData(model, 2, 0, "A2");
  setModelData(model, 2, 1, "B2");

  /*
   * -----
   * |S| |
   * -----
   * | | |
   * -----
   * |S|S|
   * -----
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 1), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A1"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B1"));
  /*
   * -----
   * | |S|
   * -----
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 1), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.rowCount(), 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RemoveRowsTest test;

  return QTest::qExec(&test, argc, argv);
}
