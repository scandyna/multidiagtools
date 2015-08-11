/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlCopierTest.h"
#include "mdtSqlFieldSetupData.h"
#include "mdtApplication.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QVariant>
#include <QTableView>
#include <QComboBox>
#include <QTreeView>

#include <QDebug>

void mdtSqlCopierTest::initTestCase()
{
  createTestDatabase();
}

void mdtSqlCopierTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.filePath());
}

/*
 * Tests implemtations
 */

// void mdtSqlCopierTest::sqlFieldTableModelTest()
// {
//   mdtSqlFieldSetupTableModel model;
//   QTableView tableView;
//   QTreeView treeView;
//   QComboBox cb;
//   QList<QSqlField> fields;
//   QSqlField field;
//   QSqlIndex pk;
// 
//   /*
//    * Setup fields
//    */
//   // Id_PK
//   field = QSqlField();
//   field.setName("Id_PK");
//   field.setType(QVariant::Int);
//   field.setAutoValue(true);
//   fields.append(field);
//   pk.append(field);
//   // Name
//   field = QSqlField();
//   field.setName("Name");
//   field.setType(QVariant::String);
//   field.setLength(100);
//   fields.append(field);
//   /*
//    * Setup model and views
//    */
//   model.setFieldList(fields);
//   model.setPrimaryKey(pk);
//   tableView.setModel(&model);
//   tableView.resize(400, 200);
//   tableView.show();
//   treeView.setModel(&model);
//   treeView.show();
//   cb.setModel(&model);
//   cb.setModelColumn(0);
//   cb.setInsertPolicy(QComboBox::InsertAtCurrent);
//   cb.setEditable(true);
//   cb.show();
//   /*
//    * Check getting field
//    */
//   QVERIFY(!model.field(-1).isValid());
//   QVERIFY(model.field(0).isValid());
//   QCOMPARE(model.field(0).name(), QString("Id_PK"));
//   QVERIFY(model.isPartOfPrimaryKey(0));
//   QVERIFY(model.field(1).isValid());
//   QCOMPARE(model.field(1).name(), QString("Name"));
//   QVERIFY(!model.isPartOfPrimaryKey(1));
//   QVERIFY(!model.field(20).isValid());
//   QVERIFY(!model.isPartOfPrimaryKey(20));
//   // Check with combo box
//   cb.setCurrentIndex(0);
//   QVERIFY(model.currentField(&cb).isValid());
//   QCOMPARE(model.currentField(&cb).name(), QString("Id_PK"));
//   cb.setCurrentIndex(1);
//   QVERIFY(model.currentField(&cb).isValid());
//   QCOMPARE(model.currentField(&cb).name(), QString("Name"));
//   cb.setCurrentIndex(-1);
//   QVERIFY(!model.currentField(&cb).isValid());
// 
//   /*
//    * Play
//    */
//   
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
// }

void mdtSqlCopierTest::sqlFieldSetupDataTest()
{
  mdtSqlFieldSetupData data;

  // Initial state
  QVERIFY(data.isNull());
  QVERIFY(data.editionMode == mdtSqlFieldSetupEditionMode_t::Selection);
  // Set
  data.tableName = "Clien_tbl";
  QVERIFY(data.isNull());
  data.field.setName("Id_PK");
  QVERIFY(data.isNull());
  data.field.setType(QVariant::Int);
  QVERIFY(!data.isNull());
  data.isPartOfPrimaryKey = true;
  data.editionMode = mdtSqlFieldSetupEditionMode_t::Creation;
  // Clear
  data.clear();
  QVERIFY(data.tableName.isEmpty());
  QVERIFY(data.isPartOfPrimaryKey == false);
  QVERIFY(data.field.name().isEmpty());
  QVERIFY(data.field.type() == QVariant::Invalid);
  QVERIFY(data.editionMode == mdtSqlFieldSetupEditionMode_t::Selection);
  QVERIFY(data.isNull());
}

// void mdtSqlCopierTest::sqlFieldSetupWidgetTest()
// {
//   mdtSqlFieldSetupWidget w(nullptr);
//   mdtSqlFieldSetupData data;
// 
//   data.tableName = "Client_tbl";
//   data.editionMode = mdtSqlFieldSetupEditionMode_t::Edition;
//   QVERIFY(w.setData(data, pvDatabaseManager.database()));
//   w.show();
// 
//   /*
//    * Play
//    */
//   while(w.isVisible()){
//     QTest::qWait(500);
//   }
// }


/*
 * Test data base manipulation methods
 */

void mdtSqlCopierTest::createTestDatabase()
{
  QTemporaryFile dbFile;
  mdtSqlSchemaTable table;
  QSqlField field;

  /*
   * Create and open database
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  pvDbFileInfo.setFile(dbFile.fileName() + ".db");
  QVERIFY(pvDatabaseManager.createDatabaseSqlite(pvDbFileInfo, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(pvDatabaseManager.database().isOpen());
  /*
   * Create tables
   */
  // Create Client_tbl
  table.clear();
  table.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field = QSqlField();
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  // Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(100);
  table.addField(field, false);
  QVERIFY(pvDatabaseManager.createTable(table, mdtSqlDatabaseManager::OverwriteExisting));

}

void mdtSqlCopierTest::populateTestDatabase()
{

}

void mdtSqlCopierTest::clearTestDatabaseData()
{

}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSqlCopierTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
