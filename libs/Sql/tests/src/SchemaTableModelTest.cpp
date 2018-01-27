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
#include "SchemaTableModelTest.h"
#include "Mdt/Sql/Schema/TableModel.h"
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

void SchemaTableModelTest::initTestCase()
{
}

void SchemaTableModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaTableModelTest::tableModelTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::TableModel;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::PrimaryKey;

  QModelIndex index;
  /*
   * Initial state
   */
  TableModel model;
  QCOMPARE(model.rowCount(), 0);
  /*
   * Setup views
   */
  QTableView tableView;
  tableView.setModel(&model);
  tableView.resize(400, 200);
  QTreeView treeView;
  treeView.setModel(&model);
  treeView.resize(400, 200);
  QComboBox comboBox;
  comboBox.setModel(&model);
  comboBox.setModelColumn(TableModel::FieldNameColumn);
  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  Name.setRequired(true);
  /*
   * Setup a table
   */
  Table Client_tbl;
  Client_tbl.setTableName("Client_tbl");
  Client_tbl.setAutoIncrementPrimaryKey("Id_PK");
  Client_tbl.addField(Name);
  /*
   * Set to model and check
   */
  QCOMPARE(model.columnCount(), 7);
  model.setTable(Client_tbl);
  // Id_PK
  index = model.index(0, TableModel::FieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  index = model.index(0, TableModel::FieldTypeColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("INTEGER"));
  index = model.index(0, TableModel::PkFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(0, TableModel::AiFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(0, TableModel::NotNullFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(0, TableModel::UniqueFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(0, TableModel::DefaultValueColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("NULL"));
  // Name
  index = model.index(1, TableModel::FieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Name"));
  index = model.index(1, TableModel::FieldTypeColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("VARCHAR(100)"));
  index = model.index(1, TableModel::PkFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(""));
  index = model.index(1, TableModel::AiFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(""));
  index = model.index(1, TableModel::NotNullFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(1, TableModel::UniqueFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(""));
  // Remarks

  /*
   * Check with Client_tbl defined in Schema
   */
  model.setTable(Schema::Client().toTable());
  QCOMPARE(model.rowCount(), 4);
  /*
   * Play
   */
  tableView.show();
  tableView.resizeColumnsToContents();
  tableView.resizeRowsToContents();
  treeView.show();
  treeView.resizeColumnToContents(0);
  treeView.resizeColumnToContents(1);
  treeView.resizeColumnToContents(2);
  treeView.resizeColumnToContents(3);
  treeView.resizeColumnToContents(4);
  treeView.resizeColumnToContents(5);
  comboBox.show();
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}
