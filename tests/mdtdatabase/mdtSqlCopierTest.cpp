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
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtSqlDatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierThread.h"
#include "mdtSqlDatabaseCopierMapping.h"
#include "mdtSqlDatabaseCopierMappingModel.h"
#include "mdtSqlDatabaseCopierDialog.h"
#include "mdtSqlCsvStringImportTableMapping.h"
#include "mdtComboBoxItemDelegate.h"
#include "mdtProgressBarItemDelegate.h"
#include "mdtProgressValue.h"
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

#include <QClipboard>
#include <QMimeData>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextBlock>
#include <QTextFragment>

#include <QDebug>

void mdtSqlCopierTest::sandbox()
{
  auto *clipboard = QApplication::clipboard();
  QVERIFY(clipboard != nullptr);
  auto *mimeData = clipboard->mimeData();
  QVERIFY(mimeData != nullptr);
  QTextEdit textEdit;
  QTextDocument doc;

  

  qDebug() << "Clipboard formats:";
  for(const auto & format : mimeData->formats()){
    qDebug() << format;
  }
  /**
  qDebug() << "data:";
  doc.setHtml(mimeData->data("text/html"));
  QTextFrame *root = doc.rootFrame();
  
  for(auto it = root->begin(); !it.atEnd(); ++it){
    QTextFrame *frame = it.currentFrame();
    if(frame != nullptr){
      auto *table = qobject_cast<QTextTable*>(frame);
      if(table != nullptr){
        for(int row = 0; row < table->rows(); ++row){
          for(int col = 0; col < table->columns(); ++col){
            QTextTableCell cell = table->cellAt(row, col);
            for(auto tableIt = cell.begin(); !tableIt.atEnd(); ++tableIt){
              QTextBlock textBlock = tableIt.currentBlock();
              for(auto blockIt = textBlock.begin(); !blockIt.atEnd(); ++blockIt){
                QTextFragment fragment = blockIt.fragment();
                qDebug() << fragment.text();
              }
            }
          }
        }
      }
    }
  }
  */
  
  ///textEdit.setText(mimeData->data("text/richtext"));
  ///textEdit.setHtml(mimeData->data("text/html"));
  textEdit.setPlainText(mimeData->data("text/plain"));
  
  textEdit.show();
  while(textEdit.isVisible()){
    QTest::qWait(500);
  }
}



/*
 * Populate Client_tbl with test data
 */
class clientTableTestDataSet
{
 public:

  clientTableTestDataSet(const QSqlDatabase & db)
   : pvDatabase(db)
  {
    Q_ASSERT(db.isValid());
  }
  ~clientTableTestDataSet()
  {
    clear();
  }
  bool populate();
  void clear();

 private:

  mdtSqlTablePopulationSchema pvPopulationSchema;
  QSqlDatabase pvDatabase;
};

bool clientTableTestDataSet::populate()
{
  QSqlQuery query(pvDatabase);
  QString sql;

  pvPopulationSchema.clear();
  pvPopulationSchema.setName("clientTableTestDataSet");
  pvPopulationSchema.setTableName("Client_tbl");
  pvPopulationSchema.addFieldName("Id_PK");
  pvPopulationSchema.addFieldName("Name");
  pvPopulationSchema.addFieldName("FieldA");
  pvPopulationSchema.addFieldName("FieldB");
  // Add data
  pvPopulationSchema.currentRowData() << 1 << "Name 1" << "FieldA 1" << "FieldB 1";
  pvPopulationSchema.commitCurrentRowData();
  pvPopulationSchema.currentRowData() << 2 << "Name 2" << "FieldA 2" << "FieldB 2";
  pvPopulationSchema.commitCurrentRowData();
  // Insert to Client_tbl
  for(int row = 0; row < pvPopulationSchema.rowDataCount(); ++row){
    sql = pvPopulationSchema.sqlForInsert(pvDatabase.driver());
    if(!query.prepare(sql)){
      qDebug() << "Prepare for insertion into Client_tbl failed, error: " << query.lastError();
      return false;
    }
    for(const auto & data : pvPopulationSchema.rowData(row)){
      query.addBindValue(data);
    }
    if(!query.exec()){
      qDebug() << "Insertion into Client_tbl failed, error: " << query.lastError();
      return false;
    }
  }

  return true;
}

void clientTableTestDataSet::clear()
{
  QSqlQuery query(pvDatabase);
  QString sql;
  int lastRow = pvPopulationSchema.rowDataCount() - 1;

  // Build SQL
  sql = "DELETE FROM Client_tbl WHERE Id_PK IN(";
  for(int row = 0; row < lastRow; ++row){
    sql += pvPopulationSchema.rowData(row).at(0).toString() + ",";
  }
  sql += pvPopulationSchema.rowData(lastRow).at(0).toString() + ")";
  // Remove data
  if(!query.exec(sql)){
    qDebug() << "Removing test data from Client_tbl failed, error: " << query.lastError();
  }
}

/*
 * Test implementation
 */

void mdtSqlCopierTest::initTestCase()
{
  createTestDatabase();
}

void mdtSqlCopierTest::cleanupTestCase()
{
  ///QFile::remove(pvDbFileInfo.filePath());
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
  QVERIFY(data.mappingState == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(data.isNull());
  // Set
  data.sourceFieldIndex = 0;
  QVERIFY(data.isNull());
  data.destinationFieldIndex = 0;
  QVERIFY(!data.isNull());
  data.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
  // Clear
  data.clear();
  QCOMPARE(data.sourceFieldIndex, -1);
  QCOMPARE(data.destinationFieldIndex, -1);
  QVERIFY(data.mappingState == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(data.isNull());
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingTest()
{
  mdtSqlDatabaseCopierTableMapping mapping;

  /*
   * Initial state
   */
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);

  /*
   * Setup databases and tables
   */
  QVERIFY(mapping.setSourceTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  // Check attributes without any mapping set
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldName(0).isNull());
  QVERIFY(mapping.sourceFieldName(1).isNull());
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id_PK");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QVERIFY(mapping.sourceFieldName(1).isNull());
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - Client_tbl.Name -> Client2_tbl.Name
   */
  mapping.setSourceField(1, "Name");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldB -> Client2_tbl.FieldA
   */
  mapping.setSourceField(2, "FieldB");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldName(2), QString("FieldB"));
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  mapping.setSourceField(3, "FieldA");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldName(2), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldName(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Reset
   */
  mapping.resetFieldMapping();
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldName(0).isNull());
  QVERIFY(mapping.sourceFieldName(1).isNull());
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.sourceFieldName(3), QString("FieldB"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);

}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingStateTest()
{
  mdtSqlDatabaseCopierTableMapping tm;

  /*
   * Initial state
   */
  QCOMPARE(tm.fieldCount(), 0);
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source and destination tables
   * and set mapping by name.
   */
  QVERIFY(tm.setSourceTable("Client_tbl", pvDatabase));
  QVERIFY(tm.setDestinationTable("Client2_tbl", pvDatabase));
  QCOMPARE(tm.fieldCount(), 4);
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Map by name and check: we know that Client_tbl and Client2_tbl have exactly the same schema
   */
  tm.generateFieldMappingByName();
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  /*
   * Setup a valid mapping "by hand" and check state
   */
  // Clear previous mapping
  tm.resetFieldMapping();
  QCOMPARE(tm.fieldCount(), 4);
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  tm.setSourceField(0, "Id_PK");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  tm.setSourceField(1, "Name");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  tm.setSourceField(2, "FieldA");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  tm.setSourceField(3, "FieldB");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  /*
   * Map incompatible field types
   */
  tm.setSourceField(0, "Name");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingError);


}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingSqliteTest()
{
  mdtSqlDatabaseCopierTableMapping mapping;
  QSqlDatabase db = pvDatabase;
  QString expectedSql;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Setup databases and tables
   */
  QVERIFY(mapping.setSourceTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  /*
   * Add field mapping:
   * - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id_PK");
  // Check SQL for count in source table
  expectedSql = "SELECT COUNT(*) FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableCount(db), expectedSql);
  // Check SQL select data in source table
  expectedSql = "SELECT \"Id_PK\" FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableSelect(db), expectedSql);
  // Check SQL to insert into destination table
  expectedSql = "INSERT INTO \"Client2_tbl\" (\"Id_PK\") VALUES (?)";
  QCOMPARE(mapping.getSqlForDestinationTableInsert(db), expectedSql);
  /*
   * Add field mapping:
   * - Client_tbl.Name -> Client2_tbl.Name
   */
  mapping.setSourceField(1, "Name");
  // Check SQL for count in source table
  expectedSql = "SELECT COUNT(*) FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableCount(db), expectedSql);
  // Check SQL select data in source table
  expectedSql = "SELECT \"Id_PK\",\"Name\" FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableSelect(db), expectedSql);
  // Check SQL to insert into destination table
  expectedSql = "INSERT INTO \"Client2_tbl\" (\"Id_PK\",\"Name\") VALUES (?,?)";
  QCOMPARE(mapping.getSqlForDestinationTableInsert(db), expectedSql);
  /*
   * Add field mapping:
   * - Client_tbl.FieldB -> Client2_tbl.FieldA
   */
  mapping.setSourceField(2, "FieldB");
  // Check SQL select data in source table
  expectedSql = "SELECT \"Id_PK\",\"Name\",\"FieldB\" FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableSelect(db), expectedSql);
  // Check SQL to insert into destination table
  expectedSql = "INSERT INTO \"Client2_tbl\" (\"Id_PK\",\"Name\",\"FieldA\") VALUES (?,?,?)";
  QCOMPARE(mapping.getSqlForDestinationTableInsert(db), expectedSql);
  /*
   * Add field mapping:
   * - Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  mapping.setSourceField(3, "FieldA");
  // Check SQL select data in source table
  expectedSql = "SELECT \"Id_PK\",\"Name\",\"FieldB\",\"FieldA\" FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableSelect(db), expectedSql);
  // Check SQL to insert into destination table
  expectedSql = "INSERT INTO \"Client2_tbl\" (\"Id_PK\",\"Name\",\"FieldA\",\"FieldB\") VALUES (?,?,?,?)";
  QCOMPARE(mapping.getSqlForDestinationTableInsert(db), expectedSql);

}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlDatabaseCopierTableMappingModel model;
  const int sourceFieldNameColumn = 0;
  const int destinationFieldNameColumn = 2;
  QModelIndex index;
  mdtSqlDatabaseCopierTableMapping tm;
  mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.setItemDelegateForColumn(sourceFieldNameColumn, delegate);
  tableView.resize(600, 200);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Check by generating by name
   */
  // Set tables and generate field mapping
  QVERIFY(model.setSourceTable("Client_tbl", pvDatabase, delegate));
  QVERIFY(model.setDestinationTable("Client2_tbl", pvDatabase));
  model.generateFieldMappingByName();
  // Check table names
  QCOMPARE(model.sourceTableName(), QString("Client_tbl"));
  QCOMPARE(model.destinationTableName(), QString("Client2_tbl"));
  // Check row 0
  index = model.index(0, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  index = model.index(0, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  // Check row 1
  index = model.index(1, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Name"));
  index = model.index(1, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Name"));
  // Check row 2
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));
  index = model.index(2, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));
  // Check row 3
  index = model.index(3, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldB"));
  index = model.index(3, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldB"));
  /*
   * Check updating mapping
   */
  tm = model.mapping();
  QCOMPARE(tm.fieldCount(), 4);
  // Update: source FieldB -> destination FieldA
  tm.setSourceField(2, "FieldB");
  // Update: source FieldA -> destination FieldB
  tm.setSourceField(3, "FieldA");
  // Update model
  model.setMapping(tm);
  // Check row 2
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldB"));
  index = model.index(2, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));
  // Check row 3
  index = model.index(3, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));
  index = model.index(3, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldB"));
  /*
   * Check selecting a field in source table
   */
  // For row 2, we set back: source FieldA -> destination FieldA
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "FieldA"));
  // Check row 2
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));
  index = model.index(2, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldA"));

  /*
   * Play
   */
  /*
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingDialogTest()
{
  mdtSqlDatabaseCopierTableMappingDialog dialog;
  mdtSqlDatabaseCopierTableMapping mapping;
  QStringList sourceTables;

  QVERIFY(mapping.setSourceTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  mapping.generateFieldMappingByName();

  sourceTables << "Client_tbl" << "Client2_tbl";
  dialog.setSourceTables(pvDatabase, sourceTables);
  dialog.setMapping(mapping);
  dialog.exec();
}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingTest()
{
  mdtSqlCsvStringImportTableMapping mapping;
  QString csvString;
  mdtCsvParserSettings csvSettings;

  /*
   * Prepare CSV source string
   */
  csvString  = "Id,Name,FieldA,FieldB\n";
  csvString += "1,Name 1,A1,B1\n";
  /*
   * Initial state
   */
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvString(csvString, csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  // Check attributes without any mapping set
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldName(0).isNull());
  QVERIFY(mapping.sourceFieldName(1).isNull());
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check also source and destination type names
  QVERIFY(mapping.sourceFieldTypeName(0).isNull());
  QCOMPARE(mapping.destinationFieldTypeName(0), QString("INTEGER"));
  /*
   * Set a field mapping:
   *  - CSV.Id -> Client_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id"));
  QVERIFY(mapping.sourceFieldName(1).isNull());
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeName(0), QString("Integer"));
  QCOMPARE(mapping.destinationFieldTypeName(0), QString("INTEGER"));
  /*
   * Set a field mapping:
   *  - CSV.Name -> Client_tbl.Name
   */
  mapping.setSourceField(1, "Name");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QVERIFY(mapping.sourceFieldName(2).isNull());
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.FieldB -> Client_tbl.FieldA
   */
  mapping.setSourceField(2, "FieldB");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldName(2), QString("FieldB"));
  QVERIFY(mapping.sourceFieldName(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.FieldA -> Client_tbl.FieldB
   */
  mapping.setSourceField(3, "FieldA");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldName(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldName(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldName(2), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldName(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));

}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingModelTest()
{

}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingTest()
{

}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingModelTest()
{

}

void mdtSqlCopierTest::sqlDatabaseCopierMappingTest()
{
  mdtSqlDatabaseCopierMapping mapping;

  /*
   * Initial state
   */
  QCOMPARE(mapping.tableMappingCount(), 0);
  QCOMPARE(mapping.tableMappingList().size(), 0);
  /*
   * Set destination database
   */
  QVERIFY(mapping.setDestinationDatabase(pvDatabase));
  // Check attributes without any mapping set
  QCOMPARE(mapping.tableMappingCount(), 2);
  QCOMPARE(mapping.tableMappingList().size(), 2);
  // Note: tables are sorted, and '_' is after '2' in ascii
  QCOMPARE(mapping.destinationTableName(0), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Client_tbl"));
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  /*
   * Set source database
   */
  QVERIFY(mapping.setSourceDatabase(pvDatabase));
  // Check attributes without any mapping set
  QCOMPARE(mapping.tableMappingCount(), 2);
  QCOMPARE(mapping.tableMappingList().size(), 2);
  // Note: tables are sorted, and '_' is after '2' in ascii
  QCOMPARE(mapping.destinationTableName(0), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Client_tbl"));
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  /*
   * Edit table mapping:
   *  Table Client_tbl -> Client2_tbl
   *  Fields:
   *   Client_tbl.Id_PK -> Client2_tbl.Id_PK
   *   Client_tbl.Name -> Client2_tbl.Name
   *   Client_tbl.FieldB -> Client2_tbl.FieldA
   *   Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  auto tm = mapping.tableMapping(0);
  QVERIFY(tm.setSourceTable("Client_tbl", pvDatabase));
  tm.setSourceField(0, "Id_PK");
  tm.setSourceField(1, "Name");
  tm.setSourceField(2, "FieldB");
  tm.setSourceField(3, "FieldA");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  mapping.setTableMapping(0, tm);
  QCOMPARE(mapping.tableMappingCount(), 2);
  QCOMPARE(mapping.tableMappingList().size(), 2);
  QCOMPARE(mapping.sourceTableName(0), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableName(0), QString("Client2_tbl"));
  /*
   * Reset mapping
   */
  QVERIFY(mapping.resetTableMapping());
  // Note: tables are sorted, and '_' is after '2' in ascii
  QCOMPARE(mapping.destinationTableName(0), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Client_tbl"));
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  /*
   * Generate mapping by names
   */
  QVERIFY(mapping.generateTableMappingByName());
  QCOMPARE(mapping.destinationTableName(0), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Client_tbl"));
  QCOMPARE(mapping.sourceTableName(0), QString("Client2_tbl"));
  QCOMPARE(mapping.sourceTableName(1), QString("Client_tbl"));
}

void mdtSqlCopierTest::sqlDatabaseCopierMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlDatabaseCopierMappingModel model;
  const int sourceTableNameColumn = 0;
  const int destinationTableNameColumn = 1;
  QModelIndex index;
  ///mdtSqlDatabaseCopierMapping dbm;
  mdtSqlDatabaseCopierTableMapping tm;
  ///mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);
  mdtProgressBarItemDelegate *progressDelegate = new mdtProgressBarItemDelegate(&tableView);
  mdtProgressValue<int> progress;

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  ///tableView.setItemDelegateForColumn(2, delegate);
  tableView.setItemDelegateForColumn(3, progressDelegate);
  tableView.resize(600, 150);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Check generating mapping by name
   */
  // Set databases and generate by name
  QVERIFY(model.setSourceDatabase(pvDatabase));
  QVERIFY(model.setDestinationDatabase(pvDatabase));
  QVERIFY(model.generateTableMappingByName());
  // Check row 0
  index = model.index(0, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client2_tbl"));
  index = model.index(0, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client2_tbl"));
  // Check row 1
  index = model.index(1, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl"));
  index = model.index(1, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl"));
  /*
   * Check that table mapping where generated for both database mappings
   */
  // Check row 0
  tm = model.tableMapping(0);
  QCOMPARE(tm.sourceTableName(), QString("Client2_tbl"));
  QCOMPARE(tm.destinationTableName(), QString("Client2_tbl"));
  QCOMPARE(tm.fieldCount(), 4);
  QCOMPARE(tm.destinationFieldName(0), QString("Id_PK"));
  // Check row 1
  tm = model.tableMapping(1);
  QCOMPARE(tm.sourceTableName(), QString("Client_tbl"));
  QCOMPARE(tm.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(tm.fieldCount(), 4);
  QCOMPARE(tm.destinationFieldName(0), QString("Id_PK"));

  /** \todo
   * Check updating model
   */
  tm = model.tableMapping(0);
  

  
  /*
   * Check updating table copy progress and status
   */
  QCOMPARE(model.rowCount(), 2);
  // Progress of row 0
  progress.setRange(0, 10);
  progress.setValue(2);
  model.setTableCopyProgress(0, progress.scaledValue());
  index = model.index(0, 3);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(20));
  // Progress of row 1
  progress.setRange(0, 1000);
  progress.setValue(500);
  model.setTableCopyProgress(1, progress.scaledValue());
  index = model.index(1, 3);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(50));
  // Clear
  model.clearCopyStatusAndProgress();
  index = model.index(0, 3);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  index = model.index(1, 3);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));

  /*
   * Play
   */
  /*
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtSqlCopierTest::sqlDatabaseCopierDialogTest()
{
  mdtSqlDatabaseCopierDialog dialog;
  QSqlDatabase db = pvDatabase;
  clientTableTestDataSet dataset(db);

  QVERIFY(db.isValid());
  /*
   * Insert some test data into Client_tbl
   */
  QVERIFY(dataset.populate());
  /*
   * Setup and show dialog
   */
  //dialog.exec();
}


void mdtSqlCopierTest::sqlDatabaseCopierThreadTest()
{
  mdtSqlDatabaseCopierThread thread;
  mdtSqlDatabaseCopierMapping mapping;
  QSqlDatabase db = pvDatabase;
  clientTableTestDataSet dataset(db);
  QSqlQuery query(db);

  QVERIFY(db.isValid());
  /*
   * Insert some test data into Client_tbl
   */
  QVERIFY(dataset.populate());
  /*
   * Check that we have no data in Client2_tbl
   */
  QVERIFY(query.exec("SELECT * FROM Client2_tbl"));
  QVERIFY(!query.next());
  /*
   * Setup database mapping
   */
  QVERIFY(mapping.setSourceDatabase(db));
  QVERIFY(mapping.setDestinationDatabase(db));
  QVERIFY(mapping.tableMappingCount() > 0);
  /*
   * Edit table mapping:
   *  Table Client_tbl -> Client2_tbl
   *  Fields:
   *   Client_tbl.Id_PK -> Client2_tbl.Id_PK
   *   Client_tbl.Name -> Client2_tbl.Name
   *   Client_tbl.FieldB -> Client2_tbl.FieldA
   *   Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  auto tm = mapping.tableMapping(0);
  QVERIFY(tm.setSourceTable("Client_tbl", pvDatabase));
  tm.setSourceField(0, "Id_PK");
  tm.setSourceField(1, "Name");
  tm.setSourceField(2, "FieldB");
  tm.setSourceField(3, "FieldA");
  QVERIFY(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  mapping.setTableMapping(0, tm);
  QCOMPARE(mapping.tableMappingCount(), 2);
  QCOMPARE(mapping.tableMappingList().size(), 2);
  /*
   * Run copy
   */
  thread.copyData(mapping);
  thread.wait();
  /*
   * Check that copy was done
   */
  QVERIFY(query.exec("SELECT * FROM Client2_tbl"));
  // Row for Id_PK = 1
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("Name 1"));
  QCOMPARE(query.value(2), QVariant("FieldB 1"));
  QCOMPARE(query.value(3), QVariant("FieldA 1"));
  // Row for Id_PK = 2
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("Name 2"));
  QCOMPARE(query.value(2), QVariant("FieldB 2"));
  QCOMPARE(query.value(3), QVariant("FieldA 2"));
  /*
   * Cleanup
   */
  QVERIFY(query.exec("DELETE FROM Client2_tbl"));
}



/*
 * Test data base manipulation methods
 */

void mdtSqlCopierTest::createTestDatabase()
{
  mdtSqlSchemaTable table;
  mdtSqlDatabaseSchema s;
  ///mdtSqlForeignKey fk;
  mdtSqlField field;

  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());

  /*
   * Create tables
   */
  // Create Client_tbl
  table.clear();
  table.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  table.addField(field, false);
  // FieldA
  field.clear();
  field.setName("FieldA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldB
  field.clear();
  field.setName("FieldB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  s.addTable(table);
  // Create Client2_tbl
  table.clear();
  table.setTableName("Client2_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  table.addField(field, false);
  // FieldA
  field.clear();
  field.setName("FieldA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldB
  field.clear();
  field.setName("FieldB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  s.addTable(table);
  // Create schema
  QVERIFY(s.createSchema(pvDatabase));
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
