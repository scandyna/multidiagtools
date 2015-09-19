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

#include "mdtComboBoxItemDelegate.h"

#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtSqlDatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierMapping.h"
#include "mdtSqlDatabaseCopierMappingModel.h"
#include "mdtSqlDatabaseCopierDialog.h"

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
#include <memory>

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


void mdtSqlCopierTest::sqlFieldSetupDataTest()
{
  mdtSqlFieldSetupData data;

  // Initial state
  QVERIFY(data.isNull());
//   QVERIFY(data.editionMode == mdtSqlFieldSetupEditionMode_t::Selection);
  // Set
//   data.tableName = "Client_tbl";
//   QVERIFY(data.isNull());
  data.field.setName("Id_PK");
  QVERIFY(data.isNull());
  data.field.setType(QVariant::Int);
  QVERIFY(!data.isNull());
  data.isPartOfPrimaryKey = true;
//   data.editionMode = mdtSqlFieldSetupEditionMode_t::Creation;
  // Clear
  data.clear();
//   QVERIFY(data.tableName.isEmpty());
  QVERIFY(data.isPartOfPrimaryKey == false);
  QVERIFY(data.field.name().isEmpty());
  QVERIFY(data.field.type() == QVariant::Invalid);
//   QVERIFY(data.editionMode == mdtSqlFieldSetupEditionMode_t::Selection);
  QVERIFY(data.isNull());
}

void mdtSqlCopierTest::fieldMappingDataTest()
{
  mdtSqlCopierFieldMapping data;

  // Initial state
  QCOMPARE(data.sourceFieldIndex, -1);
  QCOMPARE(data.destinationFieldIndex, -1);
  QVERIFY(data.isNull());
  // Set
  data.sourceFieldIndex = 0;
  QVERIFY(data.isNull());
  data.destinationFieldIndex = 0;
  QVERIFY(!data.isNull());
  // Clear
  data.clear();
  QCOMPARE(data.sourceFieldIndex, -1);
  QCOMPARE(data.destinationFieldIndex, -1);
  QVERIFY(data.isNull());
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingTest()
{
  mdtSqlDatabaseCopierTableMapping mapping;
  mdtSqlCopierFieldMapping fm;

  /*
   * Initial state
   */
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);

  /*
   * Setup databases and tables
   */
//   QVERIFY(mapping.setSourceDatabase(pvDatabaseManager.database()));
//   QVERIFY(mapping.setDestinationDatabase(pvDatabaseManager.database()));
  QVERIFY(mapping.setSourceTable("Client_tbl", pvDatabaseManager.database()));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabaseManager.database()));
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  // Check attributes without any mapping set
  QCOMPARE(mapping.fieldCount(), 2);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QVERIFY(mapping.destinationFieldName(0).isNull());
  QVERIFY(mapping.destinationFieldName(1).isNull());
  /*
   * Set a field mapping:
   *  - Client_tbl.Id_PK -> Client2_tbl.Name
   */
  mapping.setDestinationField(0, "Name");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 2);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Name"));
  QVERIFY(mapping.destinationFieldName(1).isNull());
  /*
   * Add a field mapping:
   *  - Client_tbl.Name -> Client2_tbl.Id_PK
   */
  mapping.setDestinationField(1, "Id_PK");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 2);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Id_PK"));
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 2);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);

}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlDatabaseCopierTableMappingModel model;
  mdtSqlDatabaseCopierTableMapping mapping;
  mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.setItemDelegateForColumn(2, delegate);
  tableView.resize(600, 150);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Setup model
   */
//   QVERIFY(model.setSourceDatabase(pvDatabaseManager.database()));
//   QVERIFY(model.setDestinationDatabase(pvDatabaseManager.database()));
  QVERIFY(model.setSourceTable("Client_tbl", pvDatabaseManager.database()));
  QVERIFY(model.setDestinationTable("Client2_tbl", pvDatabaseManager.database(), delegate));
  model.generateFieldMappingByName();
  tableView.resizeColumnsToContents();
  

  /*
   * Play
   */
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingDialogTest()
{
  mdtSqlDatabaseCopierTableMappingDialog dialog;
  mdtSqlDatabaseCopierTableMapping mapping;

  QVERIFY(mapping.setSourceTable("Client_tbl", pvDatabaseManager.database()));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabaseManager.database()));
  mapping.generateFieldMappingByName();

  dialog.setMapping(mapping);
  dialog.exec();
}

void mdtSqlCopierTest::sqlDatabaseCopierMappingTest()
{
  mdtSqlDatabaseCopierMapping mapping;

  /*
   * Initial state
   */
  QCOMPARE(mapping.tableMappingCount(), 0);
  /*
   * Set source database
   */
  QVERIFY(mapping.setSourceDatabase(pvDatabaseManager.database()));
  // Check attributes without any mapping set
  QCOMPARE(mapping.tableMappingCount(), 2);
  QCOMPARE(mapping.sourceTableName(0), QString("Client_tbl"));
  QCOMPARE(mapping.sourceTableName(1), QString("Client2_tbl"));
  
}

void mdtSqlCopierTest::sqlDatabaseCopierMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlDatabaseCopierMappingModel model;
  mdtSqlDatabaseCopierMapping mapping;
  mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.setItemDelegateForColumn(2, delegate);
  tableView.resize(600, 150);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Setup model
   */
  QVERIFY(model.setSourceDatabase(pvDatabaseManager.database()));
  QVERIFY(model.setDestinationDatabase(pvDatabaseManager.database()));
  QVERIFY(model.generateTableMappingByName());
  tableView.resizeColumnsToContents();
  

  /*
   * Play
   */
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::sqlDatabaseCopierDialogTest()
{
  mdtSqlDatabaseCopierDialog dialog;

  dialog.exec();
}


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
  // Create Client2_tbl
  table.clear();
  table.setTableName("Client2_tbl", "UTF8");
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
