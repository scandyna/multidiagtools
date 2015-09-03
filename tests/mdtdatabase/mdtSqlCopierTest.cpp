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
#include "mdtSqlFieldMappingDialog.h"
#include "mdtSqlTableMappingWidgetItem.h"
#include "mdtSqlTableMappingWidget.h"
#include "mdtSqlCopierCodecSettings.h"
#include "mdtSqlCopierSqliteDatabaseTableCodecSettings.h"
#include "mdtSqlCopierCodecSettingsWidget.h"
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

void mdtSqlCopierTest::codecSettingsTest()
{
  std::unique_ptr<mdtSqlCopierCodecSettings> cs;
  QVERIFY(!cs);

  /*
   * Base codec
   */
  mdtSqlCopierCodecSettings baseCs;
  QVERIFY(baseCs.type() == mdtSqlCopierCodecSettings::UnknownCodec);
  baseCs.setConnectionName("FakeCn");
  QVERIFY(baseCs.connectionName().isEmpty());
  /*
   * SQLite database table codec settings
   */
  cs.reset(new mdtSqlCopierSqliteDatabaseTableCodecSettings);
  QVERIFY(cs->type() == mdtSqlCopierCodecSettings::SqliteCodec);
  cs->setConnectionName("cnn1");
  QCOMPARE(cs->connectionName(), QString("cnn1"));
}

void mdtSqlCopierTest::codecSettingsWidgetTest()
{
  mdtSqlCopierCodecSettingsWidget w;

  /*
   * Play
   */
  w.show();
  while(w.isVisible()){
    QTest::qWait(500);
  }
}


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
  mdtSqlFieldMappingData data;

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

void mdtSqlCopierTest::fieldMappingDialogTest()
{
  mdtSqlFieldMappingDialog dialog;
  mdtSqlFieldMappingData mapping;

  QVERIFY(dialog.setSourceTable("Client_tbl", pvDatabaseManager.database()));
  QVERIFY(dialog.setDestinationTable("Client2_tbl", pvDatabaseManager.database()));
  mapping.sourceFieldIndex = 1;
  mapping.destinationFieldIndex = 1;
  dialog.setMapping(mapping);
  dialog.exec();
  QCOMPARE(dialog.mapping().sourceFieldIndex, 1);
  QCOMPARE(dialog.mapping().destinationFieldIndex, 1);
}

// void mdtSqlCopierTest::tableMappingModelTest()
// {
//   mdtSqlTableMappingModel tmm;
//   QTableView tableView;
//   QTreeView treeView;
// 
//   tableView.setModel(&tmm);
//   treeView.setModel(&tmm);
//   
//   tableView.resize(300, 200);
//   tableView.show();
//   treeView.show();
// 
//   /*
//    * Play
//    */
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
// }

void mdtSqlCopierTest::tableMappingWidgetItemTest()
{
  mdtSqlTableMappingWidget tmw;
  mdtSqlTableMappingWidgetItem item(nullptr, &tmw);

  item.show();

  /*
   * Play
   */
  while(item.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::tableMappingWidgetTest()
{
  mdtSqlTableMappingWidget tmw;

  tmw.show();

  /*
   * Play
   */
  while(tmw.isVisible()){
    QTest::qWait(500);
  }
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
