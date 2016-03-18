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
#include "mdtSqlCopierTest.h"
#include "mdtSqlFieldSetupData.h"
#include "mdtApplication.h"
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlViewSchema.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include "mdt/sql/copier/CopyHelper.h"
#include "mdt/sql/copier/TableMappingItem.h"
#include "mdt/sql/copier/FieldMapping.h"
#include "mdt/sql/copier/FixedValue.h"
#include "mdt/sql/copier/ExpressionMatchItemModel.h"
#include "mdt/sql/copier/RelatedTableInsertExpression.h"
#include "mdt/sql/copier/RelatedTableInsertMatchItemModel.h"
#include "mdt/sql/copier/RelatedTableInsertExpressionDialog.h"
#include "mdt/sql/copier/UniqueInsertCriteria.h"
#include "mdt/sql/copier/UniqueInsertCriteriaDialog.h"
#include "mdt/sql/copier/TableMappingEditHelper.h"
#include "mdt/sql/copier/DatabaseCopierTableMapping.h"
#include "mdt/sql/copier/DatabaseCopierTableMappingModel.h"
#include "mdt/sql/copier/DatabaseCopierTableMappingDialog.h"
#include "mdt/sql/copier/DatabaseMapping.h"
#include "mdt/sql/copier/DatabaseMappingModel.h"
#include "mdt/sql/copier/DatabaseCopyThread.h"
#include "mdt/sql/copier/DatabaseCopyDialog.h"
#include "mdt/sql/copier/CsvStringImportTableMapping.h"
#include "mdt/sql/copier/CsvFileImportTableMapping.h"
#include "mdt/sql/copier/CsvStringImportTableMappingModel.h"
#include "mdt/sql/copier/CsvFileImportTableMappingModel.h"
#include "mdt/sql/copier/CsvFileImportTableMappingDialog.h"
#include "mdtComboBoxItemDelegate.h"
#include "mdtProgressBarItemDelegate.h"
#include "mdtProgressValue.h"
#include <QtTest>
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
#include <QAbstractItemView>
#include <QColor>
#include <memory>
#include <initializer_list>

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

// void mdtSqlCopierTest::sandbox()
// {
//   auto *clipboard = QApplication::clipboard();
//   QVERIFY(clipboard != nullptr);
//   auto *mimeData = clipboard->mimeData();
//   QVERIFY(mimeData != nullptr);
//   QTextEdit textEdit;
//   QTextDocument doc;
// 
//   
// 
//   qDebug() << "Clipboard formats:";
//   for(const auto & format : mimeData->formats()){
//     qDebug() << format;
//   }
//   /**
//   qDebug() << "data:";
//   doc.setHtml(mimeData->data("text/html"));
//   QTextFrame *root = doc.rootFrame();
//   
//   for(auto it = root->begin(); !it.atEnd(); ++it){
//     QTextFrame *frame = it.currentFrame();
//     if(frame != nullptr){
//       auto *table = qobject_cast<QTextTable*>(frame);
//       if(table != nullptr){
//         for(int row = 0; row < table->rows(); ++row){
//           for(int col = 0; col < table->columns(); ++col){
//             QTextTableCell cell = table->cellAt(row, col);
//             for(auto tableIt = cell.begin(); !tableIt.atEnd(); ++tableIt){
//               QTextBlock textBlock = tableIt.currentBlock();
//               for(auto blockIt = textBlock.begin(); !blockIt.atEnd(); ++blockIt){
//                 QTextFragment fragment = blockIt.fragment();
//                 qDebug() << fragment.text();
//               }
//             }
//           }
//         }
//       }
//     }
//   }
//   */
//   
//   ///textEdit.setText(mimeData->data("text/richtext"));
//   ///textEdit.setHtml(mimeData->data("text/html"));
//   textEdit.setPlainText(mimeData->data("text/plain"));
//   
//   textEdit.show();
//   while(textEdit.isVisible()){
//     QTest::qWait(500);
//   }
// }

void mdtSqlCopierTest::sandbox()
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT OR IGNORE INTO Client_tbl (Id_PK,Name) VALUES ( (SELECT Id_PK FROM Client_tbl WHERE Name = :name) ,:name)";
  QVERIFY(query.prepare(sql));
  query.bindValue(":name", "Name 1");
  qDebug() << "Map:\n" << query.boundValues();
  
  QVERIFY(query.exec());
  QVERIFY(query.prepare(sql));
  query.bindValue(":name", "Name 1");
  QVERIFY(query.exec());

  

  sql = "INSERT INTO Client_tbl (Id_PK,Name,FieldA) VALUES (?,'Fixed',?)";
  QVERIFY(query.prepare(sql));
  query.bindValue(0, 2);
  query.bindValue(1, "Field A 2");
  qDebug() << "Map:\n" << query.boundValues();
  
  QVERIFY(query.exec());
  sql = "INSERT INTO Client_tbl (Id_PK,Name,FieldA) VALUES ('3','Fixed',?)";
  QVERIFY(query.prepare(sql));
  query.bindValue(0, "Field A 3");
  qDebug() << "Map:\n" << query.boundValues();
  
  QVERIFY(query.exec());

  query.exec("SELECT * FROM Client_tbl");
  while(query.next()){
    qDebug() << query.value(0) << "|" << query.value(1) << "|" << query.value(2) << "|" << query.value(3);
  }
  
  qDebug() << "TEST - SELECT test ...";
  query.prepare("SELECT * FROM Client_tbl WHERE Name = :0 AND FieldA = :1");
  query.bindValue(":0", "Fixed");
  query.bindValue(":1", "Field A 3");
  QVERIFY(query.exec());
  while(query.next()){
    qDebug() << query.value(0) << "|" << query.value(1) << "|" << query.value(2) << "|" << query.value(3);
  }
  qDebug() << "TEST - SELECT test DONE";

  
  QVERIFY(query.exec("DELETE FROM Client_tbl"));
  

}

void mdtSqlCopierTest::bechSandBox()
{
  std::vector<QString> inLst;
  QStringList lst;
  ///QVector<QString> lst;
  const int N = 100;

  for(int i = 0; i < N; ++i){
    inLst.push_back(QString("Field%0").arg(i));
  }
  qDebug() << "Insert benchmark...";
  QBENCHMARK{
    lst.clear();
    for(const auto & s : inLst){
      lst.append(s);
    }
  }
  qDebug() << lst.size();
  qDebug() << "Search benchmark...";
  QString match = QString("Field%0").arg(N-1);
  int idx = -1;
  QBENCHMARK{
    int i = 0;
    for(const auto & s : lst){
      if(s == match){
        idx = i;
        break;
      }
      ++i;
    }
  }
  qDebug() << "Found '" << match << "' at idx: " << idx;
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
 * Populate Address_tbl with test data
 */
class addressTableTestDataSet
{
 public:

  addressTableTestDataSet(const QSqlDatabase & db)
   : pvDatabase(db)
  {
    Q_ASSERT(db.isValid());
  }
  ~addressTableTestDataSet()
  {
    clear();
  }
  bool populate();
  void clear();

 private:

  mdtSqlTablePopulationSchema pvPopulationSchema;
  QSqlDatabase pvDatabase;
};

bool addressTableTestDataSet::populate()
{
  QSqlQuery query(pvDatabase);
  QString sql;

  pvPopulationSchema.clear();
  pvPopulationSchema.setName("addressTableTestDataSet");
  pvPopulationSchema.setTableName("Address_tbl");
  pvPopulationSchema.addFieldName("Id_PK");
  pvPopulationSchema.addFieldName("Client_Id_FK");
  pvPopulationSchema.addFieldName("Street");
  // Add data
  pvPopulationSchema.currentRowData() << 11 << 1 << "Address 11";
  pvPopulationSchema.commitCurrentRowData();
  pvPopulationSchema.currentRowData() << 12 << 1 << "Address 12";
  pvPopulationSchema.commitCurrentRowData();
  pvPopulationSchema.currentRowData() << 21 << 2 << "Address 21";
  pvPopulationSchema.commitCurrentRowData();
  // Insert to Address_tbl
  for(int row = 0; row < pvPopulationSchema.rowDataCount(); ++row){
    sql = pvPopulationSchema.sqlForInsert(pvDatabase.driver());
    if(!query.prepare(sql)){
      qDebug() << "Prepare for insertion into Address_tbl failed, error: " << query.lastError();
      return false;
    }
    for(const auto & data : pvPopulationSchema.rowData(row)){
      query.addBindValue(data);
    }
    if(!query.exec()){
      qDebug() << "Insertion into Address_tbl failed, error: " << query.lastError();
      return false;
    }
  }

  return true;
}

void addressTableTestDataSet::clear()
{
  QSqlQuery query(pvDatabase);
  QString sql;
  int lastRow = pvPopulationSchema.rowDataCount() - 1;

  // Build SQL
  sql = "DELETE FROM Address_tbl WHERE Id_PK IN(";
  for(int row = 0; row < lastRow; ++row){
    sql += pvPopulationSchema.rowData(row).at(0).toString() + ",";
  }
  sql += pvPopulationSchema.rowData(lastRow).at(0).toString() + ")";
  // Remove data
  if(!query.exec(sql)){
    qDebug() << "Removing test data from Address_tbl failed, error: " << query.lastError();
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

void mdtSqlCopierTest::fieldMappingTest()
{
  using mdt::sql::copier::FieldMapping;
  using mdt::sql::copier::TableMappingItemState;

  /*
   * Initial state
   */
  FieldMapping fm;
  QCOMPARE(fm.sourceFieldIndexCount(), 0);
  QCOMPARE(fm.destinationFieldIndexList().count(), 0);
  QVERIFY(fm.mappingState() == TableMappingItemState::MappingNotSet);
  QVERIFY(fm.isNull());
  /*
   * Set
   */
  fm.setFieldMapping(1, 2);
  QCOMPARE(fm.sourceFieldIndexCount(), 1);
  QCOMPARE(fm.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm.destinationFieldIndexList().at(0), 2);
  QVERIFY(!fm.isNull());
  fm.setMappingState(TableMappingItemState::MappingComplete);
  QVERIFY(fm.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Clone
   */
  FieldMapping *fmc = fm.clone();
  QVERIFY(fmc != nullptr);
  QCOMPARE(fmc->sourceFieldIndexCount(), 1);
  QCOMPARE(fmc->sourceFieldIndexList().at(0), 1);
  QCOMPARE(fmc->destinationFieldIndexCount(), 1);
  QCOMPARE(fmc->destinationFieldIndexList().at(0), 2);
  /*
   * Clear
   */
  fm.clear();
  QCOMPARE(fm.sourceFieldIndexCount(), 0);
//   QCOMPARE(fm.sourceFieldIndex(), -1);
  QCOMPARE(fm.destinationFieldIndexList().count(), 0);
  QVERIFY(fm.mappingState() == TableMappingItemState::MappingNotSet);
  QVERIFY(fm.isNull());
  /*
   * Copy of FieldMapping is not allowed
   */
  // FieldMapping fm2(fm);        // Does not compile
  // FieldMapping fm3; fm3 = fm;  // Does not compile

}

void mdtSqlCopierTest::fixedValueTest()
{
  using mdt::sql::copier::FixedValue;
  using mdt::sql::copier::TableMappingItemState;

  /*
   * Initial state
   */
  FixedValue fv;
  QVERIFY(fv.fixedValue().isNull());
  QVERIFY(fv.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(fv.destinationFieldIndexList().count(), 0);
  QVERIFY(fv.isNull());
  /*
   * Set
   */
  fv.setFixedValue("Fixed", 3);
  QCOMPARE(fv.fixedValue(), QVariant("Fixed"));
  QCOMPARE(fv.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv.destinationFieldIndexList().at(0), 3);
  QVERIFY(!fv.isNull());
  fv.setMappingState(TableMappingItemState::MappingComplete);
  QVERIFY(fv.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Clone
   */
  FixedValue *fvc = fv.clone();
  QVERIFY(fvc != nullptr);
  QCOMPARE(fvc->fixedValue(), QVariant("Fixed"));
  QCOMPARE(fvc->destinationFieldIndexList().count(), 1);
  QCOMPARE(fvc->destinationFieldIndexList().at(0), 3);
  /*
   * Clear
   */
  fv.clear();
  QVERIFY(fv.fixedValue().isNull());
  QCOMPARE(fv.destinationFieldIndexList().count(), 0);
  QVERIFY(fv.isNull());
  QVERIFY(fv.mappingState() == TableMappingItemState::MappingNotSet);
  /*
   * Copy of FixedValue is not allowed
   */
  // FixedValue fv2(fv);        // Does not compile
  // FixedValue fv3; fv3 = fv;  // Does not compile
}

void mdtSqlCopierTest::expressionMatchItemTest()
{
  using mdt::sql::copier::ExpressionMatchItem;

  // Constructs
  ExpressionMatchItem item1(-1, -1);
  QVERIFY(item1.isNull());
  ExpressionMatchItem item2(0, -1);
  QVERIFY(item2.isNull());
  ExpressionMatchItem item3(-1, 0);
  QVERIFY(item3.isNull());
  // Construct a not null item
  ExpressionMatchItem item4(1, 2);
  QCOMPARE(item4.sourceValueFieldIndex, 1);
  QCOMPARE(item4.destinationFieldIndex, 2);
  QVERIFY(item4.operatorWithPrevious == mdtSqlWhereOperator::Null);
  QVERIFY(!item4.isNull());
  // Construct a non null item with operator
  ExpressionMatchItem item5(mdtSqlWhereOperator::And, 3, 4);
  QCOMPARE(item5.sourceValueFieldIndex, 3);
  QCOMPARE(item5.destinationFieldIndex, 4);
  QVERIFY(item5.operatorWithPrevious == mdtSqlWhereOperator::And);
  QVERIFY(!item5.isNull());
}

void mdtSqlCopierTest::expressionMatchItemModelTest()
{
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::ExpressionMatchItemModel;
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  QSqlDatabase db = pvDatabase;
  auto mapping = std::make_shared<DatabaseCopierTableMapping>();
  std::vector<ExpressionMatchItem> itemList;
  ExpressionMatchItemModel model(mapping);
  const int sourceFieldColumn = 3;
  const int destinationFieldColumn = 1;
  QTableView tableView;
  QTreeView treeView;
  QModelIndex index;

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
//   tableView.setItemDelegateForColumn(sourceTypeColumn, sourceTypeDelegate);
//   tableView.setItemDelegateForColumn(sourceFieldNameColumn, sourceFieldNameDelegate);
  tableView.resize(800, 200);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();
  /*
   * Setup table mapping
   */
  mapping->setSourceDatabase(db);
  QVERIFY(mapping->setSourceTable("Client_tbl"));
  QVERIFY(mapping->setDestinationTable("Client2_tbl", db));
  /*
   * Add a match item:
   *  Client2_tbl.FieldB = Client2_tbl.FieldA
   */
  itemList.emplace_back(2, 3);
  model.setExpressionMatchItemList(itemList);
  QCOMPARE(model.rowCount(), 1);
  // Check OP
  index = model.index(0, 0);
  QVERIFY(model.data(index).isNull());
  // Check destination field name
  index = model.index(0, sourceFieldColumn);
  QCOMPARE(model.data(index), QVariant("FieldA"));
  // Check source value field name
  index = model.index(0, destinationFieldColumn);
  QCOMPARE(model.data(index), QVariant("FieldB"));
  /*
   * Add a match item:
   *  Client2_tbl.Name = Client2_tbl.Name
   */
  itemList.emplace_back(mdtSqlWhereOperator::And, 1, 1);
  model.setExpressionMatchItemList(itemList);
  QCOMPARE(model.rowCount(), 2);
  // Check OP
  index = model.index(1, 0);
  QCOMPARE(model.data(index), QVariant("AND"));
  // Check destination field name
  index = model.index(1, sourceFieldColumn);
  QCOMPARE(model.data(index), QVariant("Name"));
  // Check source value field name
  index = model.index(1, destinationFieldColumn);
  QCOMPARE(model.data(index), QVariant("Name"));
  /*
   * Edit match item at row 0:
   *  Client2_tbl.FieldA = Client2_tbl.FieldB
   */
  // Update model
  index = model.index(0, sourceFieldColumn);
  QVERIFY(model.setData(index, "FieldB", Qt::DisplayRole));
  index = model.index(0, destinationFieldColumn);
  QVERIFY(model.setData(index, "FieldA", Qt::DisplayRole));
  // Check destination field name
  index = model.index(0, sourceFieldColumn);
  QCOMPARE(model.data(index), QVariant("FieldB"));
  // Check source value field name
  index = model.index(0, destinationFieldColumn);
  QCOMPARE(model.data(index), QVariant("FieldA"));
  /*
   * Add a match item via model:
   *  Client2_tbl.Id_PK = Client2_tbl.Id_PK
   */
  // Insert item
  QVERIFY(model.insertRows(1, 1));
  QCOMPARE(model.rowCount(), 3);
  // Update item
  index = model.index(2, sourceFieldColumn);
  QVERIFY(model.setData(index, "Id_PK", Qt::DisplayRole));
  index = model.index(2, destinationFieldColumn);
  QVERIFY(model.setData(index, "Id_PK", Qt::DisplayRole));
  // Check destination field name
  index = model.index(2, sourceFieldColumn);
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  // Check source value field name
  index = model.index(2, destinationFieldColumn);
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  /*
   * Remove item et row 1
   */
  QVERIFY(model.removeRows(1, 1));
  QCOMPARE(model.rowCount(), 2);

  /*
   * Play
   */
  while(tableView.isVisible()){
    QTest::qWait(500);
  }

  /*
   * Clear
   */
  model.clearMatchItemList();
  QCOMPARE(model.rowCount(), 0);
}

void mdtSqlCopierTest::relatedTableInsertExpressionTest()
{
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::TableMappingItemState;

  std::vector<ExpressionMatchItem> matchItems;
  /*
   * Initial state
   */
  RelatedTableInsertExpression exp;
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(exp.sourceFieldIndexCount(), 0);
  QCOMPARE(exp.destinationFieldIndexCount(), 0);
  QVERIFY(exp.destinationRelatedTableName().isEmpty());
  QVERIFY(exp.destinationRelatedTableKey().isEmpty());
  QCOMPARE(exp.matchItemsCount(), 0);
  QVERIFY(exp.isNull());
  /*
   * Setup expression
   */
  // Destination field indexes
  exp.addDestinationFieldIndex(11);
  QCOMPARE(exp.destinationFieldIndexCount(), 1);
  QCOMPARE(exp.destinationFieldIndexList().at(0), 11);
  QVERIFY(exp.isNull());
  // Destination related table name
  exp.setDestinationRelatedTableName("Client2_tbl");
  QCOMPARE(exp.destinationRelatedTableName(), QString("Client2_tbl"));
  QVERIFY(exp.isNull());
  // Destination related table key
  exp.addDestinationRelatedFieldIndex(21);
  QCOMPARE(exp.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp.destinationRelatedTableKey().at(0), 21);
  QVERIFY(exp.isNull());
  // Match item
  matchItems.clear();
  matchItems.emplace_back(31, 41);
  exp.setMatchItems(matchItems);
  QCOMPARE(exp.matchItemsCount(), 1);
  QCOMPARE(exp.matchItems().at(0).sourceValueFieldIndex, 31);
  QCOMPARE(exp.matchItems().at(0).destinationFieldIndex, 41);
  QVERIFY(!exp.isNull());
  // Check that setting a null match item is reflected in isNull()
  matchItems.emplace_back(mdtSqlWhereOperator::And, -1, 0);
  exp.setMatchItems(matchItems);
  QCOMPARE(exp.matchItemsCount(), 2);
  QVERIFY(exp.isNull());
  // Set valid match items for the rest of tests
  matchItems.clear();
  matchItems.emplace_back(31, 41);
  exp.setMatchItems(matchItems);
  QCOMPARE(exp.matchItemsCount(), 1);
  QCOMPARE(exp.matchItems().at(0).sourceValueFieldIndex, 31);
  QCOMPARE(exp.matchItems().at(0).destinationFieldIndex, 41);
  QVERIFY(!exp.isNull());
  // Mapping state
  exp.setMappingState(TableMappingItemState::MappingComplete);
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Copy construction
   */
  RelatedTableInsertExpression exp2(exp);
  QCOMPARE(exp2.destinationFieldIndexCount(), 1);
  QCOMPARE(exp2.destinationFieldIndexList().at(0), 11);
  QCOMPARE(exp2.destinationRelatedTableName(), QString("Client2_tbl"));
  QCOMPARE(exp2.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp2.destinationRelatedTableKey().at(0), 21);
  QCOMPARE(exp2.matchItemsCount(), 1);
  QCOMPARE(exp2.matchItems().at(0).sourceValueFieldIndex, 31);
  QCOMPARE(exp2.matchItems().at(0).destinationFieldIndex, 41);
  QVERIFY(!exp2.isNull());
  QVERIFY(exp2.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Copy assignment
   */
  // Create exp3 and set it (make shure compiler don't use copy constructor)
  RelatedTableInsertExpression exp3;
  exp3.addDestinationFieldIndex(31);
  QCOMPARE(exp3.destinationFieldIndexCount(), 1);
  QCOMPARE(exp3.destinationFieldIndexList().at(0), 31);
  // Assign and check
  exp3 = exp;
  QCOMPARE(exp3.destinationFieldIndexCount(), 1);
  QCOMPARE(exp3.destinationFieldIndexList().at(0), 11);
  QCOMPARE(exp3.destinationRelatedTableName(), QString("Client2_tbl"));
  QCOMPARE(exp3.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp3.destinationRelatedTableKey().at(0), 21);
  QCOMPARE(exp3.matchItemsCount(), 1);
  QCOMPARE(exp3.matchItems().at(0).sourceValueFieldIndex, 31);
  QCOMPARE(exp3.matchItems().at(0).destinationFieldIndex, 41);
  QVERIFY(!exp3.isNull());
  QVERIFY(exp3.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Clear
   */
  exp.clear();
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(exp.sourceFieldIndexCount(), 0);
  QCOMPARE(exp.destinationFieldIndexCount(), 0);
  QVERIFY(exp.destinationRelatedTableName().isEmpty());
  QVERIFY(exp.destinationRelatedTableKey().isEmpty());
  QCOMPARE(exp.matchItemsCount(), 0);
  QVERIFY(exp.isNull());
}

void mdtSqlCopierTest::relatedTableInsertMatchItemModelTest()
{
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::RelatedTableInsertMatchItemModel;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  QSqlDatabase db = pvDatabase;
  auto mapping = std::make_shared<DatabaseCopierTableMapping>();
  RelatedTableInsertMatchItemModel model(mapping);
  std::vector<ExpressionMatchItem> itemList;
  const int sourceFieldColumn = 3;
  const int destinationFieldColumn = 1;
  QTableView tableView;
  QTreeView treeView;
  QModelIndex index;

  /*
   * Setup table mapping
   */
  mapping->setSourceDatabase(db);
  QVERIFY(mapping->setSourceTable("Address_Client_view"));
  QVERIFY(mapping->setDestinationTable("Address2_tbl", db));
  /*
   * Check attributes
   */
  // Check getting source related attributes
  QCOMPARE(model.sourceTableName(), QString("Address_Client_view"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().size(), 8);
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(0), QString("Id_PK"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(1), QString("Client_Id_FK"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(2), QString("Street"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(3), QString("FieldAA"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(4), QString("FieldAB"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(5), QString("ClientName"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(6), QString("ClientFieldA"));
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().at(7), QString("ClientFieldB"));
  // Set destination related table
  QVERIFY(model.setDestinationRelatedTable("Client2_tbl"));
  // Check getting destination related attributes
  QCOMPARE(model.destinationTableName(), QString("Client2_tbl"));
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().size(), 4);
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().at(1), QString("Name"));
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().at(2), QString("FieldA"));
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().at(3), QString("FieldB"));
  /*
   * Set a match item:
   *  Client2_tbl.Name = Address_Client_view.ClientName
   */
  itemList.clear();
  itemList.emplace_back(5, 1);
  model.setExpressionMatchItemList(itemList);
  QCOMPARE(model.rowCount(), 1);
  // Check related source value field name
  index = model.index(0, sourceFieldColumn);
  QCOMPARE(model.data(index), QVariant("ClientName"));
  // Check related destination field name
  index = model.index(0, destinationFieldColumn);
  QCOMPARE(model.data(index), QVariant("Name"));
  /*
   * Clear
   */
  model.clear();
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.sourceFieldCount(), 8);
  QCOMPARE(model.destinationFieldCount(), 0);
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().size(), 8);
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().size(), 0);
  /*
   * Setup model from a expression
   */
  // Setup expression
  RelatedTableInsertExpression exp;
  exp.setDestinationRelatedTableName("Client2_tbl");
  std::vector<ExpressionMatchItem> matchItems;
  matchItems.emplace_back(7, 3);
  exp.setMatchItems(matchItems);
  // Set expression to model and check
  QVERIFY(model.setExpression(exp));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.sourceFieldCount(), 8);
  QCOMPARE(model.destinationFieldCount(), 4);
//   QCOMPARE(model.getSourceRelatedTableFieldNameList().size(), 8);
  QCOMPARE(model.getDestinationRelatedTableFieldNameList().size(), 4);
  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
//   tableView.setItemDelegateForColumn(sourceTypeColumn, sourceTypeDelegate);
//   tableView.setItemDelegateForColumn(sourceFieldNameColumn, sourceFieldNameDelegate);
  tableView.resize(800, 200);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Play
   */
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::relatedTableInsertExpressionDialogTest()
{
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::RelatedTableInsertExpressionDialog;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  QSqlDatabase db = pvDatabase;
  RelatedTableInsertExpression exp;
  TableMappingItem tmItem;
  auto mapping = std::make_shared<DatabaseCopierTableMapping>();

  /*
   * Setup table mapping
   */
  mapping->setSourceDatabase(db);
  QVERIFY(mapping->setSourceTable("Address_Client_view"));
  QVERIFY(mapping->setDestinationTable("Address2_tbl", db));
  /*
   * Update table mapping with expression
   */
  // Setup expression
  exp.clear();
  exp.addDestinationFieldIndex(1);
  exp.setDestinationRelatedTableName("Client2_tbl");
  exp.addDestinationRelatedFieldIndex(0);
  std::vector<ExpressionMatchItem> matchItems;
  matchItems.emplace_back(5, 1);
  exp.setMatchItems(matchItems);
  QVERIFY(!exp.isNull());
  // Set expression to table mapping
  tmItem.setRelatedTableInsertExpression(exp);
  QVERIFY(tmItem.mappingState() == TableMappingItemState::MappingComplete);
  mapping->insertItem(tmItem);
  /*
   * Setup dialog
   */
  RelatedTableInsertExpressionDialog dialog(mapping);
  QVERIFY(dialog.setTableMappingItemIndex(1));
  /*
   * Check getting expression back
   */
  exp.clear();
  exp = dialog.expression();
  QCOMPARE(exp.destinationFieldIndexCount(), 1);
  QCOMPARE(exp.destinationRelatedTableName(), QString("Client2_tbl"));
  QCOMPARE(exp.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp.destinationRelatedTableKey().at(0), 0);
  QCOMPARE(exp.matchItemsCount(), 1);
  QCOMPARE(exp.matchItems().at(0).sourceValueFieldIndex, 5);
  QCOMPARE(exp.matchItems().at(0).destinationFieldIndex, 1);
  /*
   * Play
   */
  dialog.exec();
}

void mdtSqlCopierTest::uniqueInsertCriteriaTest()
{
  using mdt::sql::copier::UniqueInsertCriteria;
  using mdt::sql::copier::ExpressionMatchItem;

  UniqueInsertCriteria uic;
  std::vector<ExpressionMatchItem> matchItems;

  /*
   * Initial state
   */
  QCOMPARE(uic.matchItemsCount(), 0);
  QVERIFY(uic.isNull());
  /*
   * Set
   */
  matchItems.clear();
  matchItems.emplace_back(1, 2);
  uic.setMatchItems(matchItems);
  QCOMPARE(uic.matchItemsCount(), 1);
  QVERIFY(!uic.isNull());
  /*
   * Clear
   */
  uic.clear();
  QCOMPARE(uic.matchItemsCount(), 0);
  QVERIFY(uic.isNull());
  /*
   * Check setting invalid field indexes in match items
   */
  matchItems.clear();
  matchItems.emplace_back(-1, 2);
  uic.setMatchItems(matchItems);
  QCOMPARE(uic.matchItemsCount(), 1);
  QVERIFY(uic.isNull());
  matchItems.clear();
  matchItems.emplace_back(2, -1);
  uic.setMatchItems(matchItems);
  QCOMPARE(uic.matchItemsCount(), 1);
  QVERIFY(uic.isNull());

}

void mdtSqlCopierTest::uniqueInsertCriteriaDialogTest()
{
  using mdt::sql::copier::UniqueInsertCriteria;
  using mdt::sql::copier::UniqueInsertCriteriaDialog;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  QSqlDatabase db = pvDatabase;
  auto mapping = std::make_shared<DatabaseCopierTableMapping>();
  UniqueInsertCriteria uic;
  std::vector<ExpressionMatchItem> matchItems;

  /*
   * Setup table mapping
   */
  // Set source and destination
  mapping->setSourceDatabase(db);
  QVERIFY(mapping->setSourceTable("Client_tbl"));
  QVERIFY(mapping->setDestinationTable("Client2_tbl", db));
  /*
   * Setup criteria
   */
  matchItems.emplace_back(1, 2);
  uic.setMatchItems(matchItems);
  mapping->setUniqueInsertCriteria(uic);

  UniqueInsertCriteriaDialog dialog(mapping);

  /*
   * Play
   */
  dialog.exec();
}

void mdtSqlCopierTest::tableMappingItemTest()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
//   using mdt::sql::copier::UniqueInsertExpression;

  /*
   * Construction of field mapping item
   */
  TableMappingItem fm1(TableMappingItem::FieldMappingType);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 0);
  QVERIFY(fm1.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 0);
  QVERIFY(fm1.isNull());
  /*
   * Copy construction of field mapping item
   */
  // Set some value to fm1
  fm1.setFieldMapping(1, 11);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QVERIFY(!fm1.isNull());
  // Construct fm2 on base of fm1
  TableMappingItem fm2(fm1);
  QVERIFY(fm2.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm2.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm2.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm2.destinationFieldIndexList().at(0), 11);
  QVERIFY(!fm2.isNull());
  // Update fm2
  qDebug() << "TEST: fm2.setFieldMapping() ...";
  fm2.setFieldMapping(2, 22);
  qDebug() << "TEST: check fm1 ...";
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  qDebug() << "TEST: check fm2 ...";
  QCOMPARE(fm2.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm2.sourceFieldIndexList().at(0), 2);
  QCOMPARE(fm2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm2.destinationFieldIndexList().at(0), 22);
  qDebug() << "TEST: check fm1 and fm2 OK";
  /*
   * Copy assignment of field mapping item
   */
  // Create fm3
  TableMappingItem fm3(TableMappingItem::FieldMappingType);
  QVERIFY(fm3.isNull());
  // Assign
  fm3 = fm1;
  QVERIFY(fm3.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm3.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm3.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm3.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm3.destinationFieldIndexList().at(0), 11);
  // Update fm3
  fm3.setFieldMapping(3, 33);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QCOMPARE(fm3.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm3.sourceFieldIndexList().at(0), 3);
  QCOMPARE(fm3.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm3.destinationFieldIndexList().at(0), 33);
  /*
   * Copy assignment of different type
   */
  TableMappingItem fm4(TableMappingItem::FixedValueType);
  QVERIFY(fm4.type() == TableMappingItem::FixedValueType);
  QVERIFY(fm4.isNull());
  // Assign
  fm4 = fm1;
  QVERIFY(fm4.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm4.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm4.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm4.destinationFieldIndexList().at(0), 11);
  // Update fm4
  fm4.setFieldMapping(4, 44);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QCOMPARE(fm4.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm4.sourceFieldIndexList().at(0), 4);
  QCOMPARE(fm4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm4.destinationFieldIndexList().at(0), 44);
  /*
   * Construction of fixed value item
   */
  TableMappingItem fv1(TableMappingItem::FixedValueType);
  QVERIFY(fv1.type() == TableMappingItem::FixedValueType);
  QVERIFY(fv1.isNull());
  /*
   * Copy construction of fixed value item
   */
  // Set some value to fv1
  fv1.setFixedValue("Fixed 1", 1);
  QCOMPARE(fv1.fixedValue(), QVariant("Fixed 1"));
  QCOMPARE(fv1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv1.destinationFieldIndexList().at(0), 1);
  QVERIFY(!fv1.isNull());
  // Construct fv2 on base of fv1
  TableMappingItem fv2(fv1);
  QVERIFY(fv2.type() == TableMappingItem::FixedValueType);
  QCOMPARE(fv2.fixedValue(), QVariant("Fixed 1"));
  QCOMPARE(fv2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().at(0), 1);
  QVERIFY(!fv2.isNull());
  // Update fv2
  fv2.setFixedValue("Fixed 2", 2);
  QCOMPARE(fv1.fixedValue(), QVariant("Fixed 1"));
  QCOMPARE(fv1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv1.destinationFieldIndexList().at(0), 1);
  QCOMPARE(fv2.fixedValue(), QVariant("Fixed 2"));
  QCOMPARE(fv2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().at(0), 2);
  /*
   * Copy assignment of fixes value item
   */
  // Create fv3
  TableMappingItem fv3(TableMappingItem::FixedValueType);
  QVERIFY(fv3.type() == TableMappingItem::FixedValueType);
  QVERIFY(fv3.isNull());
  // Assign
  fv3 = fv1;
  QCOMPARE(fv3.fixedValue(), QVariant("Fixed 1"));
  QCOMPARE(fv3.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv3.destinationFieldIndexList().at(0), 1);
  // Update fv3
  fv3.setFixedValue("Fixed 3", 3);
  QCOMPARE(fv1.fixedValue(), QVariant("Fixed 1"));
  QCOMPARE(fv1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv1.destinationFieldIndexList().at(0), 1);
  QCOMPARE(fv3.fixedValue(), QVariant("Fixed 3"));
  QCOMPARE(fv3.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv3.destinationFieldIndexList().at(0), 3);
  /*
   * Copy construct of fixed value on base of a field mapping
   */
  TableMappingItem fv4(fm1);
  QVERIFY(fv4.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fv4.sourceFieldIndexList().count(), 1);
  QCOMPARE(fv4.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fv4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv4.destinationFieldIndexList().at(0), 11);
  // Set fixed value to fv4
  fv4.setFixedValue("Fixed 4", 4);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QVERIFY(fv4.type() == TableMappingItem::FixedValueType);
  QCOMPARE(fv4.fixedValue(), QVariant("Fixed 4"));
  QCOMPARE(fv4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv4.destinationFieldIndexList().at(0), 4);
  /*
   * Assign fm1 -> fv2
   */
  QVERIFY(fv2.type() == TableMappingItem::FixedValueType);
  fv2 = fm1;
  QVERIFY(fv2.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fv2.sourceFieldIndexList().count(), 1);
  QCOMPARE(fv2.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fv2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().at(0), 11);
  // Set fixed value to fv2
  fv2.setFixedValue("Fixed 22", 22);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndexList().count(), 1);
  QCOMPARE(fm1.sourceFieldIndexList().at(0), 1);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QVERIFY(fv2.type() == TableMappingItem::FixedValueType);
  QCOMPARE(fv2.fixedValue(), QVariant("Fixed 22"));
  QCOMPARE(fv2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().at(0), 22);
  /*
   * Construct a unique insert expression
   */
  // Setup expression
//   UniqueInsertExpression uniqueInsertExpression;
//   uniqueInsertExpression.addDestinationFieldIndex(1);
//   uniqueInsertExpression.addDestinationFieldIndex(2);
//   uniqueInsertExpression.addMatchItem(3, 4);
//   uniqueInsertExpression.addMatchItem(mdtSqlWhereOperator::And, 5, 6);
//   // Create table mapping item
//   TableMappingItem uiexp(TableMappingItem::UniqueInsertExpressionType);
//   QVERIFY(uiexp.type() == TableMappingItem::UniqueInsertExpressionType);
//   QVERIFY(uiexp.isNull());
//   // Set expression
//   uiexp.setUniqueInsertExpression(uniqueInsertExpression);
//   QVERIFY(uiexp.type() == TableMappingItem::UniqueInsertExpressionType);
//   QCOMPARE(uiexp.destinationFieldIndexList().count(), 2);
//   QCOMPARE(uiexp.sourceFieldIndexList().count(), 0);
//   QCOMPARE(uiexp.destinationFieldIndexList().at(0), 1);
//   QCOMPARE(uiexp.destinationFieldIndexList().at(1), 2);
//   QVERIFY(!uiexp.isNull());
//   /*
//    * Check getting unique insert expression from table mapping item
//    *  when item is allready a unique expression
//    */
//   uniqueInsertExpression.clear();
//   QVERIFY(uniqueInsertExpression.isNull());
//   uniqueInsertExpression = uiexp.uniqueInsertExpression();
//   QCOMPARE(uniqueInsertExpression.destinationFieldIndexCount(), 2);
//   QCOMPARE(uniqueInsertExpression.destinationFieldIndexList().at(0), 1);
//   QCOMPARE(uniqueInsertExpression.destinationFieldIndexList().at(1), 2);
//   QCOMPARE(uniqueInsertExpression.matchItemsCount(), 2);
//   QCOMPARE(uniqueInsertExpression.matchItemAt(0).sourceValueFieldIndex, 3);
//   QCOMPARE(uniqueInsertExpression.matchItemAt(0).destinationFieldIndex, 4);
//   QCOMPARE(uniqueInsertExpression.matchItemAt(1).sourceValueFieldIndex, 5);
//   QCOMPARE(uniqueInsertExpression.matchItemAt(1).destinationFieldIndex, 6);
//   /*
//    * Check getting unique insert expression from table mapping item
//    *  when item is not a unique expression
//    */
//   // Assure test data
//   uniqueInsertExpression.clear();
//   QVERIFY(uniqueInsertExpression.isNull());
//   QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
//   QVERIFY(fm1.destinationFieldIndexList().count() > 0);
//   // Get expression
//   uniqueInsertExpression = fm1.uniqueInsertExpression();
//   // Check that this not effects fm1
//   QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
//   // Check expression
//   QCOMPARE(uniqueInsertExpression.destinationFieldIndexCount(), fm1.destinationFieldIndexList().count());
//   QVERIFY(uniqueInsertExpression.mappingState() == TableMappingItemState::MappingNotSet);
//   QVERIFY(uniqueInsertExpression.isNull());

  /*
   * Copy construction of different types
   */

  /*
   * Copy assignment of different types
   */

  /*
   * Check setting different type of one existing items object works
   */

}

void mdtSqlCopierTest::tableMappingItemRelatedTableExpTest()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::ExpressionMatchItem;

  RelatedTableInsertExpression exp;
  std::vector<ExpressionMatchItem> matchItems;

  /*
   * Construct a table mapping item of related table insert expression type
   */
  TableMappingItem tmExp0(TableMappingItem::RelatedTableInsertExpressionType);
  QVERIFY(tmExp0.type() == TableMappingItem::RelatedTableInsertExpressionType);
  QCOMPARE(tmExp0.sourceFieldIndexList().count(), 0);
  QVERIFY(tmExp0.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(tmExp0.destinationFieldIndexList().count(), 0);
  QVERIFY(tmExp0.isNull());
  /*
   * Construct a related table expression
   */
  // Setup expression
  exp.clear();
  exp.addDestinationFieldIndex(11);
  exp.setDestinationRelatedTableName("Client2_tbl");
  exp.addDestinationRelatedFieldIndex(22);
  matchItems.clear();
  matchItems.emplace_back(1, 2);
  exp.setMatchItems(matchItems);
  QVERIFY(!exp.isNull());
  // Setup table mapping item
  TableMappingItem tmExp1(TableMappingItem::RelatedTableInsertExpressionType);
  tmExp1.setRelatedTableInsertExpression(exp);
  // Check
  QVERIFY(tmExp1.type() == TableMappingItem::RelatedTableInsertExpressionType);
  QCOMPARE(tmExp1.sourceFieldIndexList().count(), 0);
  QCOMPARE(tmExp1.destinationFieldIndexList().count(), 1);
  QCOMPARE(tmExp1.destinationFieldIndexList().at(0), 11);
  /*
   * Check getting related table expression from table mapping item
   *  when item is allready a related table
   */
  exp.clear();
  exp = tmExp1.relatedTableInsertExpression();
  QCOMPARE(exp.sourceFieldIndexCount(), 0);
  QCOMPARE(exp.destinationFieldIndexCount(), 1);
  QCOMPARE(exp.destinationFieldIndexList().at(0), 11);
  QCOMPARE(exp.destinationRelatedTableName(), QString("Client2_tbl"));
  QCOMPARE(exp.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp.destinationRelatedTableKey().at(0), 22);
  QCOMPARE(exp.matchItemsCount(), 1);
  QCOMPARE(exp.matchItems().at(0).sourceValueFieldIndex, 1);
  QCOMPARE(exp.matchItems().at(0).destinationFieldIndex, 2);
  /*
   * Check getting related table expression from table mapping item
   *  when item is not a related table
   */
  // Setup a field mapping item
  TableMappingItem fm1(TableMappingItem::FieldMappingType);
  fm1.setFieldMapping(21, 31);
  //Get expression and check
  exp = fm1.relatedTableInsertExpression();
  QCOMPARE(exp.sourceFieldIndexCount(), 0);
  QCOMPARE(exp.destinationFieldIndexCount(), 1);
  QCOMPARE(exp.destinationFieldIndexList().at(0), 31);
  QVERIFY(exp.destinationRelatedTableName().isEmpty());
  /*
   * Copy construction from a expression table mapping item
   */
  TableMappingItem tmExp2(tmExp1);
  QVERIFY(tmExp2.type() == TableMappingItem::RelatedTableInsertExpressionType);
  QCOMPARE(tmExp2.sourceFieldIndexList().count(), 0);
  QCOMPARE(tmExp2.destinationFieldIndexList().count(), 1);
  QCOMPARE(tmExp2.destinationFieldIndexList().at(0), 11);
  /*
   * Copy construction of different table mapping item types
   */
  
  

  /*
   * Copy assignment of table mapping item different types
   */

}

void mdtSqlCopierTest::tableMappingEditHelperContainsDFIndexesTest()
{
  using mdt::sql::copier::TableMappingEditHelper;
  using mdt::sql::FieldIndexList;

  QFETCH(FieldIndexList, itemDfiList);
  QFETCH(FieldIndexList, dfiList);
  QFETCH(bool, expectedMatch);

  QVERIFY(TableMappingEditHelper::itemContainsDfiList(itemDfiList, dfiList) == expectedMatch);
}

void mdtSqlCopierTest::tableMappingEditHelperContainsDFIndexesTest_data()
{
  using mdt::sql::FieldIndexList;

  QTest::addColumn<FieldIndexList>("itemDfiList");
  QTest::addColumn<FieldIndexList>("dfiList");
  QTest::addColumn<bool>("expectedMatch");

  FieldIndexList itemDfiList;
  FieldIndexList dfiList;

  /*
   * Empty item
   */
  dfiList.clear();
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {0};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {1};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {2};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {0,1};
  QTest::newRow("") << itemDfiList << dfiList << false;
  /*
   * Item with one DFI
   */
  itemDfiList.clear();
  itemDfiList.append(2);
  dfiList.clear();
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {0};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {1};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {2};
  QTest::newRow("") << itemDfiList << dfiList << true;
  dfiList = {0,1};
  QTest::newRow("") << itemDfiList << dfiList << false;
  dfiList = {0,1,2};
  QTest::newRow("") << itemDfiList << dfiList << true;
}

void mdtSqlCopierTest::tableMappingEditHelperRemoveItemsTest()
{
  using mdt::sql::copier::TableMappingEditHelper;
  using mdt::sql::FieldIndexList;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::RelatedTableInsertExpression;

  QFETCH(QVector<FieldIndexList>, allItems);
  QFETCH(FieldIndexList, item);
  QFETCH(QVector<int>, expectedItemsToRemove);
  QFETCH(QVector<FieldIndexList>, expectedNewAllItems);
  QVector<TableMappingItem> tmItemsList;

  // Build a expression with DFI list for each item in allItems
  for(const auto & dfiList : allItems){
    TableMappingItem tmi(TableMappingItem::RelatedTableInsertExpressionType);
    RelatedTableInsertExpression exp;
    for(int dfi : dfiList){
      exp.addDestinationFieldIndex(dfi);
    }
    tmi.setRelatedTableInsertExpression(exp);
    tmItemsList.append(tmi);
  }
  /*
   * Check getting list of indexes to remove
   */
  // Get indexes of items to remove
  auto itemsToRemove = TableMappingEditHelper::getItemsToRemoveIndexList(item, tmItemsList, false);
  // Check
  QCOMPARE((int)itemsToRemove.size(), expectedItemsToRemove.size());
  for(int i = 0; i < expectedItemsToRemove.size(); ++i){
    QCOMPARE(itemsToRemove.at(i) , expectedItemsToRemove.at(i));
  }
  /*
   * Check removin items
   */
  // Remove items
  TableMappingEditHelper::removeItems(item, tmItemsList);
  // Check
  QCOMPARE(tmItemsList.size(), expectedNewAllItems.size());
  for(int i = 0; i < expectedNewAllItems.size(); ++i){
    QCOMPARE(tmItemsList.at(i).destinationFieldIndexList().count(), expectedNewAllItems.at(i).count());
    for(int k = 0; k < expectedNewAllItems.at(i).count(); ++k){
      QCOMPARE(tmItemsList.at(i).destinationFieldIndexList().at(k), expectedNewAllItems.at(i).at(k));
    }
  }
}

void mdtSqlCopierTest::tableMappingEditHelperRemoveItemsTest_data()
{
  using mdt::sql::FieldIndexList;

  QTest::addColumn<QVector<FieldIndexList>>("allItems");
  QTest::addColumn<FieldIndexList>("item");
  QTest::addColumn<QVector<int>>("expectedItemsToRemove");
  QTest::addColumn<QVector<FieldIndexList>>("expectedNewAllItems");

  QVector<FieldIndexList> allItems;
  FieldIndexList item;
  QVector<int> expectedItemsToRemove;
  QVector<FieldIndexList> expectedNewAllItems;
  FieldIndexList dfi;

  /*
   * Empty TM
   */
  QTest::newRow("EmptyTM") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 0   |
   * -------
   *
   * Indexes of items to remove: {0}
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * --------------
   * expectedNewAllItems (=TM after remove): {}
   */
  // Setup TM
  allItems.clear();
  dfi = {0};
  allItems << dfi;
  // Setup item to insert
  item = {0};
  // Setup expected indexes of items to remove
  expectedItemsToRemove = {0};
  // Setup expected TM after remove
  expectedNewAllItems.clear();
  // Add to check list
  QTest::newRow("[{0}]->[{}]") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  1  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 0   |
   * -------
   *
   * Indexes of items to remove: {}
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  1  |
   * --------------
   * expectedNewAllItems (=TM after remove): {}
   */
  // Setup TM
  allItems.clear();
  dfi = {1};
  allItems << dfi;
  // Setup item to insert
  item = {0};
  // Setup expected indexes of items to remove
  expectedItemsToRemove.clear();
  // Setup expected TM after remove
  expectedNewAllItems.clear();
  dfi = {1};
  expectedNewAllItems << dfi;
  // Add to check list
  QTest::newRow("[{1}]->[{1}]") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 1,2 |
   * -------
   *
   * Indexes of items to remove: {}
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   * expectedNewAllItems (=TM after remove): {}
   */
  // Setup TM
  allItems.clear();
  dfi = {0};
  allItems << dfi;
  // Setup item to insert
  item = {1,2};
  // Setup expected indexes of items to remove
  expectedItemsToRemove.clear();
  // Setup expected TM after remove
  expectedNewAllItems.clear();
  dfi = {0};
  expectedNewAllItems << dfi;
  // Add to check list
  QTest::newRow("[{0}]->[{0}]") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   * |  1   |  1  |
   * --------------
   * |  2   |  2  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 1,2 |
   * -------
   *
   * Indexes of items to remove: {1,2}
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   * expectedNewAllItems (=TM after remove): {}
   */
  // Setup TM
  allItems.clear();
  dfi = {0};
  allItems << dfi;
  dfi = {1};
  allItems << dfi;
  dfi = {2};
  allItems << dfi;
  // Setup item to insert
  item = {1,2};
  // Setup expected indexes of items to remove
  expectedItemsToRemove = {1,2};
  // Setup expected TM after remove
  expectedNewAllItems.clear();
  dfi = {0};
  expectedNewAllItems << dfi;
  // Add to check list
  QTest::newRow("[{0},{1},{2}]->[{0}]") << allItems << item << expectedItemsToRemove << expectedNewAllItems;

  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  1  |
   * --------------
   */
  allItems.clear();
  // Setup DFI
  dfi = {1};
  // Add to TM
  allItems.append(dfi);
  /*
   * Item's DFI: 0
   * Index of items to remove: -
   */
  // Setup item
  item = {0};
  // Setup expected list of indexes to remove from TM
  expectedItemsToRemove.clear();
  // Setup expected TM
  expectedNewAllItems = allItems;
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * Item's DFI: 1
   * Index of items to remove: 0
   */
  // Setup item
  item = {1};
  // Setup expected list of indexes to remove from TM
  expectedItemsToRemove.clear();
  expectedItemsToRemove << 0;
  // Setup expected TM
  expectedNewAllItems.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * Item's DFI: 0,1
   * Index of items to remove: 0
   */
  // Setup item
  item = {0,1};
  // Setup expected list of indexes to remove from TM
  expectedItemsToRemove.clear();
  expectedItemsToRemove << 0;
  // Setup expected TM
  expectedNewAllItems.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToRemove << expectedNewAllItems;
  /*
   * Item's DFI: 0,2
   * Index of items to remove: -
   */
  // Setup item
  item = {0,2};
  // Setup expected list of indexes to remove from TM
  expectedItemsToRemove.clear();
  // Setup expected TM
  expectedNewAllItems = allItems;
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToRemove << expectedNewAllItems;

}

void mdtSqlCopierTest::tableMappingEditHelperItemDfiToAddTest()
{
  using mdt::sql::copier::TableMappingEditHelper;
  using mdt::sql::FieldIndexList;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::RelatedTableInsertExpression;

  QFETCH(QVector<FieldIndexList>, allItems);
  QFETCH(FieldIndexList, item);
  QFETCH(FieldIndexList, expectedItemsToCreate);
  QVector<TableMappingItem> tmItemsList;

  // Build a expression with DFI list for each item in allItems
  for(const auto & dfiList : allItems){
    TableMappingItem tmi(TableMappingItem::RelatedTableInsertExpressionType);
    RelatedTableInsertExpression exp;
    for(int dfi : dfiList){
      exp.addDestinationFieldIndex(dfi);
    }
    tmi.setRelatedTableInsertExpression(exp);
    tmItemsList.append(tmi);
  }
  // Get items to create
  auto itemsToCreate = TableMappingEditHelper::getItemsToCreateList(item, tmItemsList);
  // Check
  QCOMPARE(itemsToCreate.count(), expectedItemsToCreate.count());
  for(int i = 0; i < expectedItemsToCreate.count(); ++i){
    QCOMPARE(itemsToCreate.at(i).destinationFieldIndexList().count(), 1);
    QCOMPARE(itemsToCreate.at(i).destinationFieldIndexList().at(0), expectedItemsToCreate.at(0));
  }
}

void mdtSqlCopierTest::tableMappingEditHelperItemDfiToAddTest_data()
{
  using mdt::sql::FieldIndexList;

  QTest::addColumn<QVector<FieldIndexList>>("allItems");
  QTest::addColumn<FieldIndexList>("item");
  QTest::addColumn<FieldIndexList>("expectedItemsToCreate");

  QVector<FieldIndexList> allItems;
  FieldIndexList item;
  FieldIndexList expectedItemsToCreate;
  FieldIndexList dfi;

  /*
   * Empty TM
   */
  QTest::newRow("EmptyTM") << allItems << item << expectedItemsToCreate;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 0   |
   * -------
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * --------------
   * allItems (=TM after remove): {}
   *
   * DFI to create: {}
   */
  // Setup item to insert
  item = {0};
  // Setup allItems
  allItems.clear();
  // Setup expected DFI to create list
  expectedItemsToCreate.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToCreate;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 1   |
   * -------
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   * allItems (=TM after remove): {0}
   *
   * DFI to create: {}
   */
  // Setup item to insert
  item = {1};
  // Setup allItems
  allItems.clear();
  dfi = {0};
  allItems << dfi;
  // Setup expected DFI to create list
  expectedItemsToCreate.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToCreate;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   * |  1   |  1  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 0   |
   * -------
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * |  1   |  1  |
   * --------------
   * allItems (=TM after remove): {}
   *
   * DFI to create: {}
   */
  // Setup item to insert
  item = {0};
  // Setup allItems
  allItems.clear();
  dfi = {1};
  allItems << dfi;
  // Setup expected DFI to create list
  expectedItemsToCreate.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToCreate;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   | 0,1 |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 1   |
   * -------
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * --------------
   * allItems (=TM after remove): {}
   *
   * DFI to create: {0}
   */
  // Setup item to insert
  item = {1};
  // Setup allItems
  allItems.clear();
  // Setup expected DFI to create list
  expectedItemsToCreate = {0};
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToCreate;
  /*
   * TM:
   * --------------
   * | Item | DFI |
   * --------------
   * |  0   |  0  |
   * --------------
   *
   * Item to insert:
   * -------
   * | DFI |
   * -------
   * | 0,1 |
   * -------
   *
   * TM after removed items that must be:
   * --------------
   * | Item | DFI |
   * --------------
   * --------------
   * allItems (=TM after remove): {}
   *
   * DFI to create: {}
   */
  // Setup item to insert
  item = {0,1};
  // Setup allItems
  allItems.clear();
  // Setup expected DFI to create list
  expectedItemsToCreate.clear();
  // Add to check list
  QTest::newRow("") << allItems << item << expectedItemsToCreate;

}

void mdtSqlCopierTest::tableMappingEditHelperInsertTest()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingEditHelper;

  QFETCH(QVector<TableMappingItem>, tmItemList);
  QFETCH(TableMappingItem, item);
  QFETCH(QVector<TableMappingItem>, expectedTmItemList);

  // Insert item
  TableMappingEditHelper::insertItem(item, tmItemList);
  // Check
  QCOMPARE(tmItemList.size(), expectedTmItemList.size());
  for(int i = 0; i < expectedTmItemList.size(); ++i){
    auto item = tmItemList.at(i);
    auto expectedItem = expectedTmItemList.at(i);
//     QCOMPARE(item.sourceFieldIndex(), expectedItem.sourceFieldIndex());
    QCOMPARE(item.sourceFieldIndexList().count(), expectedItem.sourceFieldIndexList().count());
    for(int k = 0; k < expectedItem.sourceFieldIndexList().count(); ++k){
      QCOMPARE(item.sourceFieldIndexList().at(k), expectedItem.sourceFieldIndexList().at(k));
    }
    QCOMPARE(item.destinationFieldIndexList().count(), expectedItem.destinationFieldIndexList().count());
    for(int k = 0; k < expectedItem.destinationFieldIndexList().count(); ++k){
      QCOMPARE(item.destinationFieldIndexList().at(k), expectedItem.destinationFieldIndexList().at(k));
    }
  }
}

void mdtSqlCopierTest::tableMappingEditHelperInsertTest_data()
{
  using mdt::sql::FieldIndexList;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::RelatedTableInsertExpression;

  QTest::addColumn<QVector<TableMappingItem>>("tmItemList");
  QTest::addColumn<TableMappingItem>("item");
  QTest::addColumn<QVector<TableMappingItem>>("expectedTmItemList");

  QVector<TableMappingItem> tmItemList;
  QVector<TableMappingItem> expectedTmItemList;
  TableMappingItem item;
  RelatedTableInsertExpression exp;

  /*
   * Empty TM
   */
  ///QTest::newRow("EmptyTM") << tmItemList << item << expectedTmItemList;
  /*
   * TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 10  | 0   |
   * --------------------
   *
   * Item to insert:
   * -------------
   * | SFI | DFI |
   * -------------
   * | 11  | 1   |
   * -------------
   *
   * Resulting TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 10  | 0   |
   * --------------------
   * |  1   | 11  | 1   |
   * --------------------
   */
  // Setup TM
  tmItemList.clear();
  item.setFieldMapping(10, 0);
  tmItemList << item;
  // Setup expeced resulting TM
  expectedTmItemList.clear();
  item.setFieldMapping(10, 0);
  expectedTmItemList << item;
  item.setFieldMapping(11, 1);
  expectedTmItemList << item;
  // Setup item to insert
  item.setFieldMapping(11, 1);
  // Add to check list
  QTest::newRow("[{0}]->[{0},{1}]") << tmItemList << item << expectedTmItemList;
  /*
   * TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 10  | 0   |
   * --------------------
   * |  1   | 11  | 1   |
   * --------------------
   *
   * Item to insert:
   * -------------
   * | SFI | DFI |
   * -------------
   * | 20  | 0   |
   * -------------
   *
   * Resulting TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 20  | 0   |
   * --------------------
   * |  1   | 11  | 1   |
   * --------------------
   */
  // Setup TM
  tmItemList.clear();
  item.setFieldMapping(10, 0);
  tmItemList << item;
  item.setFieldMapping(11, 1);
  tmItemList << item;
  // Setup expeced resulting TM
  expectedTmItemList.clear();
  item.setFieldMapping(20, 0);
  expectedTmItemList << item;
  item.setFieldMapping(11, 1);
  expectedTmItemList << item;
  // Setup item to insert
  item.setFieldMapping(20, 0);
  // Add to check list
  QTest::newRow("[{0},{1}]->[{0},{1}]") << tmItemList << item << expectedTmItemList;
  /*
   * TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 10  | 0   |
   * --------------------
   *
   * Item to insert:
   * -------------
   * | SFI | DFI |
   * -------------
   * | -   | 2,3 |
   * -------------
   *
   * Resulting TM:
   * --------------------
   * | Item | SFI | DFI |
   * --------------------
   * |  0   | 10  | 0   |
   * --------------------
   * |  1   | -   | 1   |
   * --------------------
   * |  2   | -   | 2,3 |
   * --------------------
   */
  // Setup TM
  tmItemList.clear();
  item.setFieldMapping(10, 0);
  tmItemList << item;
  // Setup expeced resulting TM
  expectedTmItemList.clear();
  item.setFieldMapping(10, 0);
  expectedTmItemList << item;
  item.setFieldMapping(-1, 1);
  expectedTmItemList << item;
  exp.clear();
  exp.addDestinationFieldIndex(2);
  exp.addDestinationFieldIndex(3);
  item.setRelatedTableInsertExpression(exp);
  expectedTmItemList << item;
  // Setup item to insert
  exp.clear();
  exp.addDestinationFieldIndex(2);
  exp.addDestinationFieldIndex(3);
  item.setRelatedTableInsertExpression(exp);
  // Add to check list
  QTest::newRow("[{0}]->[{0},{1},{2,3}]") << tmItemList << item << expectedTmItemList;

}

void mdtSqlCopierTest::databaseCopierTableMappingUpdateItemsTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping mapping;
  TableMappingItem item;
  RelatedTableInsertExpression rtiExp;
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup databases and tables
   */
  mapping.setSourceDatabase(pvDatabase);
  QVERIFY(mapping.setSourceTable("Client_tbl"));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setting tables will create one item per field in destination table
   */
  QCOMPARE(mapping.itemsCount(), 4);
  // Check item at index 0
  item = mapping.itemAt(0);
  QVERIFY(item.type() == TableMappingItem::FieldMappingType);
  QVERIFY(item.isNull());
  // Check item at index 1
  item = mapping.itemAt(1);
  QVERIFY(item.type() == TableMappingItem::FieldMappingType);
  QVERIFY(item.isNull());
  // Check item at index 2
  item = mapping.itemAt(2);
  QVERIFY(item.type() == TableMappingItem::FieldMappingType);
  QVERIFY(item.isNull());
  // Check item at index 3
  item = mapping.itemAt(3);
  QVERIFY(item.type() == TableMappingItem::FieldMappingType);
  QVERIFY(item.isNull());
  /*
   * Set a field mapping:
   *  - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  item.setFieldMapping(0, 0);
  mapping.insertItem(item);
  // Check
  QCOMPARE(mapping.itemsCount(), 4);
  item = mapping.itemAt(0);
  QVERIFY(item.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(item.sourceFieldIndexList().count(), 1);
  QCOMPARE(item.sourceFieldIndexList().at(0), 0);
  QCOMPARE(item.destinationFieldIndexList().count(), 1);
  QCOMPARE(item.destinationFieldIndexList().at(0), 0);
  /*
   * Set a related table insert expression:
   *  - Client2_tbl fields: FieldA, FieldB
   */
  // Setup expression
  rtiExp.clear();
  rtiExp.addDestinationFieldIndex(2);
  rtiExp.addDestinationFieldIndex(3);
  // Set expression to item and update TM
  item.setRelatedTableInsertExpression(rtiExp);
  mapping.insertItem(item);
  // Check TM
  QCOMPARE(mapping.itemsCount(), 3);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
//   QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
//   QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 2);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(1), QString("FieldB"));
  /*
   * Set a fixed value:
   *  - Client2_tbl.Name: "Fixed name"
   */
  item.setFixedValue("Fixed name", 1);
  mapping.insertItem(item);
  // Check TM
  QCOMPARE(mapping.itemsCount(), 3);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFixedValueAtItem(1), QVariant("Fixed name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 2);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(1), QString("FieldB"));
  
  /// \todo Check setting a field mapping byidirectly uie mapping.setFieldMapping()


  return;
  
  /*
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  */
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(2) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(3) == TableMappingItem::FieldMappingType);

}

void mdtSqlCopierTest::databaseCopierTableMappingTableFetchTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping mapping;
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup databases and tables
   */
  mapping.setSourceDatabase(pvDatabase);
  QVERIFY(mapping.setSourceTable("Client_tbl"));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Check getting databases object back
   */
  QCOMPARE(mapping.sourceDatabase().databaseName(), pvDatabase.databaseName());
  QCOMPARE(mapping.destinationDatabase().databaseName(), pvDatabase.databaseName());
  /*
   * Check fetching tables specific informations
   */
  // Check for source table name
  QCOMPARE(mapping.sourceTableName(), QString("Client_tbl"));
  // Check getting source table field name
  QCOMPARE(mapping.sourceTableFieldCount(), 4);
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.sourceTableFieldNameAt(3), QString("FieldB"));
  QCOMPARE(mapping.getSourceTableFieldNameList().size(), 4);
  // Check getting source table field index
  QCOMPARE(mapping.sourceTableFieldIndexOf("Id_PK"), 0);
  QCOMPARE(mapping.sourceTableFieldIndexOf("Name"), 1);
  QCOMPARE(mapping.sourceTableFieldIndexOf("FieldA"), 2);
  QCOMPARE(mapping.sourceTableFieldIndexOf("FieldB"), 3);
  // Check for destination table name
  QCOMPARE(mapping.destinationTableName(), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  // Check getting destination table field name
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));
  QCOMPARE(mapping.getDestinationTableFieldNameList().size(), 4);
  // Check getting destination table field index
  QCOMPARE(mapping.destinationTableFieldIndexOf("Id_PK"), 0);
  QCOMPARE(mapping.destinationTableFieldIndexOf("Name"), 1);
  QCOMPARE(mapping.destinationTableFieldIndexOf("FieldA"), 2);
  QCOMPARE(mapping.destinationTableFieldIndexOf("FieldB"), 3);
}

void mdtSqlCopierTest::databaseCopierTableMappingFieldMapTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping mapping;
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup databases and tables
   */
  mapping.setSourceDatabase(pvDatabase);
  QVERIFY(mapping.setSourceTable("Client_tbl"));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Check attributes without any mapping set
   */
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(2) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(3) == TableMappingItem::FieldMappingType);
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 0);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
//   QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 1);
//   QVERIFY(mapping.sourceFieldKeyTypeListAtItem(0).at(0) == TableMapping::NotAKey);
//   QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
//   QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
//   QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 1);
//   QVERIFY(mapping.sourceFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
//   QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
//   QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  /*
   * Set a field mapping:
   *  - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceFieldAtItem(0, "Id_PK");
  // Check attributes
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 0);
//   QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 1);
//   QVERIFY(mapping.sourceFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  /*
   * Set a field mapping:
   *  - Client_tbl.Name -> Client2_tbl.Name
   */
  mapping.setSourceFieldAtItem(1, "Name");
  // Check attributes
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldB -> Client2_tbl.FieldA
   */
  mapping.setSourceFieldAtItem(2, "FieldB");
  // Check attributes
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  mapping.setSourceFieldAtItem(3, "FieldA");
  // Check attributes
  QVERIFY(mapping.mappingState() == TableMapping::MappingComplete);
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingComplete);
  /*
   * Reset
   */
  mapping.resetFieldMapping();
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).at(0), QString("FieldB"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.mappingState() == TableMapping::MappingComplete);
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
}

void mdtSqlCopierTest::databaseCopierTableMappingFixedValueTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping mapping;
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup databases and tables
   */
  mapping.setSourceDatabase(pvDatabase);
  QVERIFY(mapping.setSourceTable("Client_tbl"));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Check field mapping with fixed value:
   *  - Map source field Id_PK -> destination field Id_PK
   *  - Map a fixed value -> destination Name
   */
  // Check initial state
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  /*
   * Map Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceFieldAtItem(0, "Id_PK");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  /*
   * Set a fixed value for Client2_tbl.Name
   */
  mapping.setSourceFixedValueAtItem(1, "Fixed name");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFixedValueAtItem(1), QVariant("Fixed name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
}

void mdtSqlCopierTest::databaseCopierTableMappingRelatedTableExpressionTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping mapping;
  RelatedTableInsertExpression exp;
  std::vector<ExpressionMatchItem> matchItems;
  TableMappingItem tmItem;
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup databases and tables
   */
  mapping.setSourceDatabase(pvDatabase);
  QVERIFY(mapping.setSourceTable("Address_Client_view"));
  QVERIFY(mapping.setDestinationTable("Address2_tbl", pvDatabase));
  QCOMPARE(mapping.itemsCount(), 5);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup expression:
   *  - Destination field (in Address2_tbl): 1 (Client_Id_FK)
   *  - Destination related table: Client2_tbl
   *  - Destination related key: Id_PK
   *  - Match criteria: Client2_tbl.Name = Address_Client_view.ClientName
   */
  exp.clear();
  exp.addDestinationFieldIndex(1);
  exp.setDestinationRelatedTableName("Client2_tbl");
  exp.addDestinationRelatedFieldIndex(0);
  matchItems.clear();
  matchItems.emplace_back(5, 1);
  exp.setMatchItems(matchItems);
  QVERIFY(!exp.isNull());
  // Setup and add table mapping item
  tmItem.setRelatedTableInsertExpression(exp);
  mapping.insertItem(tmItem);
  // Check mapping item
  QCOMPARE(mapping.itemsCount(), 5);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Client_Id_FK"));
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  /*
   * Get expression back and check
   */
  exp.clear();
  QVERIFY(exp.isNull());
  exp = mapping.itemAt(1).relatedTableInsertExpression();
  QCOMPARE(exp.destinationFieldIndexCount(), 1);
  QCOMPARE(exp.destinationFieldIndexList().at(0), 1);
  QCOMPARE(exp.destinationRelatedTableName(), QString("Client2_tbl"));
  QCOMPARE(exp.destinationRelatedTableKey().count(), 1);
  QCOMPARE(exp.destinationRelatedTableKey().at(0), 0);
  matchItems = exp.matchItems();
  QCOMPARE((int)matchItems.size(), 1);
  QCOMPARE(matchItems.at(0).sourceValueFieldIndex, 5);
  QCOMPARE(matchItems.at(0).destinationFieldIndex, 1);
  QVERIFY(!exp.isNull());
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingStateTest()
{
  using mdt::sql::copier::DatabaseCopierTableMapping;

  DatabaseCopierTableMapping tm;

  /*
   * Initial state
   */
  QCOMPARE(tm.itemsCount(), 0);
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source and destination tables
   * and set mapping by name.
   */
  tm.setSourceDatabase(pvDatabase);
  QVERIFY(tm.setSourceTable("Client_tbl"));
  QVERIFY(tm.setDestinationTable("Client2_tbl", pvDatabase));
  QCOMPARE(tm.itemsCount(), 4);
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingNotSet);
  /*
   * Map by name and check: we know that Client_tbl and Client2_tbl have exactly the same schema
   */
  tm.generateFieldMappingByName();
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingComplete);
  /*
   * Setup a valid mapping "by hand" and check state
   */
  // Clear previous mapping
  tm.resetFieldMapping();
  QCOMPARE(tm.itemsCount(), 4);
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingNotSet);
  tm.setSourceFieldAtItem(0, "Id_PK");
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  tm.setSourceFieldAtItem(1, "Name");
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  tm.setSourceFieldAtItem(2, "FieldA");
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  tm.setSourceFieldAtItem(3, "FieldB");
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingComplete);
  /*
   * Map incompatible field types
   */
  tm.setSourceFieldAtItem(0, "Name");
  QVERIFY(tm.mappingState() == DatabaseCopierTableMapping::MappingError);
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingModelTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMappingModel;
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  QTableView tableView;
  QTreeView treeView;
  DatabaseCopierTableMappingModel model;
  const int sourceTypeColumn = 0;
  const int sourceFieldNameColumn = 2;
  const int sourceFixedValueColumn = 4;
  const int destinationFieldNameColumn = 6;
  QModelIndex index;
  TableMappingItem::Type itemTyppe;
  std::shared_ptr<DatabaseCopierTableMapping> tm;
  TableMappingItem tmi;
  mdtComboBoxItemDelegate *sourceTypeDelegate = new mdtComboBoxItemDelegate(&tableView);
  mdtComboBoxItemDelegate *sourceFieldNameDelegate = new mdtComboBoxItemDelegate(&tableView);
  RelatedTableInsertExpression rtExp;
  std::vector<ExpressionMatchItem> matchItems;
  QSqlDatabase db = pvDatabase;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  QVERIFY(db.isOpen());
  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.setItemDelegateForColumn(sourceTypeColumn, sourceTypeDelegate);
  tableView.setItemDelegateForColumn(sourceFieldNameColumn, sourceFieldNameDelegate);
  tableView.resize(900, 300);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Check by generating field mapping by name
   */
  // Set tables and generate field mapping
  model.setupItemTypeDelegate(sourceTypeDelegate);
  model.setSourceDatabase(db);
  QVERIFY(model.setSourceTable("Client_tbl"));
  sourceFieldNameDelegate->clear();
  sourceFieldNameDelegate->addItem("");
  sourceFieldNameDelegate->addItems(model.mapping()->getSourceTableFieldNameList());
  QVERIFY(model.setDestinationTable("Client2_tbl", db));
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
  QCOMPARE(tm->itemsCount(), 4);
  // Update: source FieldB -> destination FieldA
  tm->setSourceFieldAtItem(2, "FieldB");
  // Update: source FieldA -> destination FieldB
  tm->setSourceFieldAtItem(3, "FieldA");
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
   * Check with a mixed type table mapping:
   * ------------------------------------------------
   * |        Source field      | Destination field |
   * | (in Address_Client_view) | (in Address2_tbl) |
   * |--------------------------|-------------------|
   * |           --             | 0 (Id_PK)         |
   * |--------------------------|-------------------|
   * | rtExp                    | 1 (Client_Id_FK)  |
   * |--------------------------|-------------------|
   * | 2 (Street)               | 2 (Street)        |
   * |--------------------------|-------------------|
   * | Fixed                    | 3 (FieldAA)       |
   * |--------------------------|-------------------|
   * | 3 (FieldAA)              | 4 (FieldAB)       |
   * |----------------------------------------------|
   */
  // Set tables
  model.setupItemTypeDelegate(sourceTypeDelegate);
  QVERIFY(model.setSourceTable("Address_Client_view"));
  sourceFieldNameDelegate->clear();
  sourceFieldNameDelegate->addItem("");
  sourceFieldNameDelegate->addItems(model.mapping()->getSourceTableFieldNameList());
  QVERIFY(model.setDestinationTable("Address2_tbl", db));
  QCOMPARE(model.rowCount(), 5);
  // Item[0] is not mapped
  /*
   * Setup expression for item[1]:
   *  - Destination field (in Address2_tbl): 1 (Client_Id_FK)
   *  - Destination related table: Client2_tbl
   *  - Destination related key: Id_PK
   *  - Match criteria: Client2_tbl.Name = Address_Client_view.ClientName
   */
  rtExp.clear();
  rtExp.addDestinationFieldIndex(1);
  rtExp.setDestinationRelatedTableName("Client2_tbl");
  rtExp.addDestinationRelatedFieldIndex(0);
  matchItems.clear();
  matchItems.emplace_back(5, 1);
  rtExp.setMatchItems(matchItems);
  QVERIFY(!rtExp.isNull());
  // Set expression to table mapping
  tmi.setRelatedTableInsertExpression(rtExp);
  model.insertItem(tmi);
  /*
   * Set field mapping for item[2]
   */
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  // Select Street
  beginEditing(tableView, index);
  sourceFieldNameDelegate->setCurrentIndex(2+1);
  endEditing(tableView, index);
  /*
   * Set fixed value for item[3]
   */
  index = model.index(3, sourceFixedValueColumn);
  QVERIFY(index.isValid());
  model.setData(index, "Fixed");
  /*
   * Set field mapping for item[4]
   */
  index = model.index(4, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  // Select Street
  beginEditing(tableView, index);
  sourceFieldNameDelegate->setCurrentIndex(3+1);
  endEditing(tableView, index);
  // Check row 0
  index = model.index(0, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  index = model.index(0, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  // Check row 1
  index = model.index(1, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  index = model.index(1, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_Id_FK"));
  // Check row 2
  index = model.index(2, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Street"));
  index = model.index(2, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Street"));
  // Check row 3
  index = model.index(3, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  index = model.index(3, sourceFixedValueColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Fixed"));
  index = model.index(3, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldAA"));
  // Check row 4
  index = model.index(4, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldAA"));
  index = model.index(4, destinationFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("FieldAB"));

  /**
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }

  
  /*
   * Check selecting a source type
   */
  index = model.index(0, sourceTypeColumn);
  QVERIFY(index.isValid());
  // Select fixed value type
  beginEditing(tableView, index);
  sourceTypeDelegate->setCurrentIndex(1);
  endEditing(tableView, index);
  // Check that source type was updated
//   sourceType = static_cast<mdtSqlCopierFieldMapping::SourceType>(model.data(index, Qt::EditRole).toInt());
//   sourceType = static_cast<SourceField::Type>(model.data(index, Qt::EditRole).toInt());
  itemTyppe = static_cast<TableMappingItem::Type>(model.data(index, Qt::EditRole).toInt());
  QVERIFY(itemTyppe == TableMappingItem::FixedValueType);
//   QVERIFY(sourceType == SourceField::SourceFixedValueType);
//   QVERIFY(sourceType == mdtSqlCopierFieldMapping::FixedValue);
  // Check getting source field name - Must return a null value
  index = model.index(0, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  // Check getting source field type name - Must return a null value
  index = model.index(0, sourceFieldNameColumn+1);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());

  /*
   * Check selecting a field in source table
   */
  // Check for row 1, witch is currently of Field type
  index = model.index(1, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Name"));
  // Select FieldA
  beginEditing(tableView, index);
  sourceFieldNameDelegate->setCurrentIndex(3);
  endEditing(tableView, index);
  // Check that source field name was updated
  QCOMPARE(model.data(index), QVariant("FieldA"));
  // Check for row 1, witch is currently of FixedValue type
  index = model.index(0, sourceFieldNameColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  // Select Name
  beginEditing(tableView, index);
  sourceFieldNameDelegate->setCurrentIndex(2);
  endEditing(tableView, index);
  // Check that nothing happened
  QVERIFY(model.data(index).isNull());



//   // For row 2, we set back: source FieldA -> destination FieldA
//   index = model.index(2, sourceFieldNameColumn);
//   QVERIFY(index.isValid());
//   QVERIFY(model.setData(index, "FieldA"));
//   // Check row 2
//   index = model.index(2, sourceFieldNameColumn);
//   QVERIFY(index.isValid());
//   QCOMPARE(model.data(index), QVariant("FieldA"));
//   index = model.index(2, destinationFieldNameColumn);
//   QVERIFY(index.isValid());
//   QCOMPARE(model.data(index), QVariant("FieldA"));

  /*
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingDialogTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  mdt::sql::copier::DatabaseCopierTableMappingDialog dialog;
  auto mapping = std::make_shared<DatabaseCopierTableMapping>();

  mapping->setSourceDatabase(pvDatabase);
  QVERIFY(mapping->setSourceTable("Client_tbl"));
  QVERIFY(mapping->setDestinationTable("Client2_tbl", pvDatabase));
  mapping->generateFieldMappingByName();
  QVERIFY(mapping->mappingState() == TableMapping::MappingComplete);

  // Set mapping and check
  dialog.setMapping(mapping);
  mapping = dialog.mapping();
  QCOMPARE(mapping->sourceTableName(), QString("Client_tbl"));
  QCOMPARE(mapping->destinationTableName(), QString("Client2_tbl"));
  QVERIFY(mapping->mappingState() == TableMapping::MappingComplete);

  /*
   * Play
   */
  dialog.exec();
}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::CsvStringImportTableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  CsvStringImportTableMapping mapping;
  QString csvString;
  mdtCsvParserSettings csvSettings;
  QStringList fieldNames;

  /*
   * Prepare CSV source string
   */
  csvString  = "Id,Name,FieldA,FieldB\n";
  csvString += "1,Name 1,A1,B1\n";
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvString(csvString, csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingNotSet);
  /*
   * Check fetching database objects
   */
  QVERIFY(!mapping.sourceDatabase().isValid());
  QCOMPARE(mapping.destinationDatabase().databaseName(), pvDatabase.databaseName());
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QString("CSV String")); /// \todo Define what name it should be
  QCOMPARE(mapping.sourceTableFieldCount(), 4);
  // Check getting source table field name
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.sourceTableFieldNameAt(3), QString("FieldB"));
  QCOMPARE(mapping.getSourceTableFieldNameList().size(), 4);
  // Check getting source table field index
  QCOMPARE(mapping.sourceTableFieldIndexOf("Id"), 0);
  QCOMPARE(mapping.sourceTableFieldIndexOf("Name"), 1);
  QCOMPARE(mapping.sourceTableFieldIndexOf("FieldA"), 2);
  QCOMPARE(mapping.sourceTableFieldIndexOf("FieldB"), 3);
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  // Check getting destination table field name
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));
  QCOMPARE(mapping.getDestinationTableFieldNameList().size(), 4);
  // Check getting destination table field index
  QCOMPARE(mapping.destinationTableFieldIndexOf("Id_PK"), 0);
  QCOMPARE(mapping.destinationTableFieldIndexOf("Name"), 1);
  QCOMPARE(mapping.destinationTableFieldIndexOf("FieldA"), 2);
  QCOMPARE(mapping.destinationTableFieldIndexOf("FieldB"), 3);
  /*
   * Check attributes without any mapping set
   */
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
//   QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(2) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(3) == TableMappingItem::FieldMappingType);
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(0).at(0) == TableMapping::NotAKey);
//   QVERIFY(mapping.sourceFieldKeyTypeAtItem(0) == TableMapping::NotAKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(1).at(0) == TableMapping::PrimaryKey);
//   QVERIFY(mapping.sourceFieldKeyTypeAtItem(1) == TableMapping::NotAKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).size(), 0);
//   QVERIFY(mapping.sourceFieldTypeNameAtItem(0).isNull());
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).at(0), QString("INTEGER"));
  // Check getting source field names
  fieldNames = mapping.getSourceTableFieldNameList();
  QCOMPARE(fieldNames.size(), 4);
  QCOMPARE(fieldNames.at(0), QString("Id"));
  QCOMPARE(fieldNames.at(1), QString("Name"));
  QCOMPARE(fieldNames.at(2), QString("FieldA"));
  QCOMPARE(fieldNames.at(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.Id -> Client_tbl.Id_PK
   */
  mapping.setSourceFieldAtItem(0, "Id");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
//   QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
//   QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).at(0), QString("Integer"));
//   QCOMPARE(mapping.sourceFieldTypeNameAtItem(0), QString("Integer"));
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).at(0), QString("INTEGER"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  /*
   * Set a field mapping:
   *  - CSV.Name -> Client_tbl.Name
   */
  mapping.setSourceFieldAtItem(1, "Name");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
//   QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
//   QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  /*
   * Set a field mapping:
   *  - CSV.FieldB -> Client_tbl.FieldA
   */
  mapping.setSourceFieldAtItem(2, "FieldB");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
//   QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
//   QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  /*
   * Set a field mapping:
   *  - CSV.FieldA -> Client_tbl.FieldB
   */
  mapping.setSourceFieldAtItem(3, "FieldA");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).at(0), QString("FieldA"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingComplete);
  /*
   * Reset
   */
  mapping.resetFieldMapping();
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
//   QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
//   QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
//   QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
//   QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
//   QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingPartial);
  // Now, manually map CSV source Id -> Client_tbl Id_PK
  mapping.setSourceFieldAtItem(0, "Id");
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingComplete);
//   QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id"));

  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == DatabaseCopierTableMapping::MappingNotSet);

}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingModelTest()
{
  using mdt::sql::copier::CsvStringImportTableMappingModel;
//   using mdt::sql::copier::CsvStringImportTableMapping;

  QTableView tableView;
  QTreeView treeView;
  CsvStringImportTableMappingModel model;
  const int sourceFieldNameColumn = 2;
  const int destinationFieldNameColumn = 5;
  QModelIndex index;
  mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);
  QString csvString;
  mdtCsvParserSettings csvSettings;

  /*
   * Prepare CSV source string
   */
  csvString  = "Id,Name,FieldA,FieldB\n";
  csvString += "1,Name 1,A1,B1\n";
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
  QVERIFY(model.setSourceCsvString(csvString, csvSettings));
  delegate->clear();
  delegate->addItem("");
  delegate->addItems(model.mapping()->getSourceTableFieldNameList());
  QVERIFY(model.setDestinationTable("Client_tbl", pvDatabase));
  model.generateFieldMappingByName();

  /*
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingTableFetchTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::CsvFileImportTableMapping;

  CsvFileImportTableMapping mapping;
  mdtCsvParserSettings csvSettings;
  QStringList fieldNames;

  /*
   * Prepare CSV source file
   */
  QTemporaryFile csvFile;
  QVERIFY(csvFile.open());
  QVERIFY(csvFile.write("Id,Name,FieldA,FieldB\n") > 0);
  QVERIFY(csvFile.write("1,Name 1,A1,B1\n") > 0);
  csvFile.close();
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvFile(csvFile, "UTF-8", csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QFileInfo(csvFile).fileName());
  QCOMPARE(mapping.sourceTableFieldCount(), 4);
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.sourceTableFieldNameAt(3), QString("FieldB"));
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));
  /*
   * Prepare a other CSV source file
   *  with different field count
   *  (Bug of 20160301)
   */
  QTemporaryFile csvFile2;
  QVERIFY(csvFile2.open());
  QVERIFY(csvFile2.write("Id,Street,Remarks\n") > 0);
  QVERIFY(csvFile2.write("1,Street 1,Remark 1\n") > 0);
  csvFile2.close();
  /*
   * Update source table and check
   */
  QVERIFY(mapping.setSourceCsvFile(csvFile2, "UTF-8", csvSettings));
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QFileInfo(csvFile2).fileName());
  QCOMPARE(mapping.sourceTableFieldCount(), 3);
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Street"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("Remarks"));
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));

}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::CsvFileImportTableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  CsvFileImportTableMapping mapping;
  mdtCsvParserSettings csvSettings;
  
  QStringList fieldNames;

  /** \todo Define how CSV field types are set
   *        F.ex. a CSV settings, with default to all fields are strings ?
   *                Also, auto-detetction ?
   *        Once done, update also this test
   */
  /*
   * Prepare CSV source file
   */
  QTemporaryFile csvFile;
  QVERIFY(csvFile.open());
  QVERIFY(csvFile.write("Id,Name,FieldA,FieldB\n") > 0);
  QVERIFY(csvFile.write("1,Name 1,A1,B1\n") > 0);
  csvFile.close();
  /*
   * Initial state
   */
  QCOMPARE(mapping.itemsCount(), 0);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvFile(csvFile, "UTF-8", csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QFileInfo(csvFile).fileName());
  QCOMPARE(mapping.sourceTableFieldCount(), 4);
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.sourceTableFieldNameAt(3), QString("FieldB"));
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));
  /*
   * Check attributes without any mapping set
   */
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(2) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(3) == TableMappingItem::FieldMappingType);
  // Check items mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).at(0), QString("INTEGER"));
  // Check getting source field names
  fieldNames = mapping.getSourceTableFieldNameList();
  QCOMPARE(fieldNames.size(), 4);
  QCOMPARE(fieldNames.at(0), QString("Id"));
  QCOMPARE(fieldNames.at(1), QString("Name"));
  QCOMPARE(fieldNames.at(2), QString("FieldA"));
  QCOMPARE(fieldNames.at(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.Id -> Client_tbl.Id_PK
   */
  mapping.setSourceFieldAtItem(0, "Id");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(0).at(0) == TableMapping::NotAKey);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).at(0), QString("String"));  /// \todo Update once format defined
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).at(0), QString("INTEGER"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingError);   /// \todo Update once format defined
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  ///QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  QVERIFY(mapping.mappingState() == TableMapping::MappingError);  /// \todo Update once format defined
  /*
   * Set a field mapping:
   *  - CSV.Name -> Client_tbl.Name
   */
  mapping.setSourceFieldAtItem(1, "Name");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(0).at(0) == TableMapping::NotAKey);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.sourceFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingError);   /// \todo Update once format defined
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingComplete);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  /// \todo Update once format defined
  ///QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  /*
   * Set a field mapping:
   *  - CSV.FieldB -> Client_tbl.FieldA
   */
  mapping.setSourceFieldAtItem(2, "FieldB");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  /// \todo Update once format defined
  ///QVERIFY(mapping.mappingState() == TableMapping::MappingPartial);
  /*
   * Set a field mapping:
   *  - CSV.FieldA -> Client_tbl.FieldB
   */
  mapping.setSourceFieldAtItem(3, "FieldA");
  // Check attributes
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).at(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).at(0), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check mapping state
  /// \todo Update once format defined
  ///QVERIFY(mapping.mappingState() == TableMapping::MappingComplete);
  /*
   * Prepare a other CSV source file
   *  with different field count
   *  (bug from 20160301)
   */
  QTemporaryFile csvFile2;
  QVERIFY(csvFile2.open());
  QVERIFY(csvFile2.write("Id,Street,Remarks\n") > 0);
  QVERIFY(csvFile2.write("1,Street 1,Remark 1\n") > 0);
  csvFile2.close();
  /*
   * Update source CSV file
   */
  QVERIFY(mapping.setSourceCsvFile(csvFile2, "UTF-8", csvSettings));
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QFileInfo(csvFile2).fileName());
  QCOMPARE(mapping.sourceTableFieldCount(), 3);
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  /*
   * Check attributes without any mapping set
   */
  QCOMPARE(mapping.itemsCount(), 4);
  QCOMPARE(mapping.sourceFieldNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(1).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(2).size(), 0);
  QCOMPARE(mapping.sourceFieldNameListAtItem(3).size(), 0);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(0).at(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(1).at(0), QString("Name"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(2).at(0), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).size(), 1);
  QCOMPARE(mapping.destinationFieldNameListAtItem(3).at(0), QString("FieldB"));
  // Check that default mapping type where choosen
  QVERIFY(mapping.itemType(0) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(1) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(2) == TableMappingItem::FieldMappingType);
  QVERIFY(mapping.itemType(3) == TableMappingItem::FieldMappingType);
  // Check field key types
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(0).size(), 0);
  QCOMPARE(mapping.sourceFieldKeyTypeListAtItem(1).size(), 0);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(0).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(0).at(0) == TableMapping::PrimaryKey);
  QCOMPARE(mapping.destinationFieldKeyTypeListAtItem(1).size(), 1);
  QVERIFY(mapping.destinationFieldKeyTypeListAtItem(1).at(0) == TableMapping::NotAKey);
  // Check also source and destination type names
  QCOMPARE(mapping.sourceFieldTypeNameListAtItem(0).size(), 0);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).size(), 1);
  QCOMPARE(mapping.destinationFieldTypeNameListAtItem(0).at(0), QString("INTEGER"));
  // Check mapping state
  QVERIFY(mapping.itemMappingState(0) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(1) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(2) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.itemMappingState(3) == TableMappingItemState::MappingNotSet);
  QVERIFY(mapping.mappingState() == TableMapping::MappingNotSet);
}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingModelTest()
{
  using mdt::sql::copier::CsvFileImportTableMappingModel;

  QTableView tableView;
  QTreeView treeView;
  CsvFileImportTableMappingModel model;
  const int sourceFieldNameColumn = 2;
  const int destinationFieldNameColumn = 5;
  QModelIndex index;
  mdtComboBoxItemDelegate *delegate = new mdtComboBoxItemDelegate(&tableView);
  mdtCsvParserSettings csvSettings;
  QTemporaryFile csvFile;

  /*
   * Prepare CSV source file
   */
  QVERIFY(csvFile.open());
  QVERIFY(csvFile.write("Id,Name,FieldA,FieldB\n") > 0);
  QVERIFY(csvFile.write("1,Name 1,A1,B1\n") > 0);
  csvFile.close();
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
  QVERIFY(model.setSourceCsvFile(csvFile, "UTF-8", csvSettings));
  delegate->clear();
  delegate->addItem("");
  delegate->addItems(model.mapping()->getSourceTableFieldNameList());
  QVERIFY(model.setDestinationTable("Client_tbl", pvDatabase));
  model.generateFieldMappingByName();

  /*
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::csvFileImportTableMappingDialogTest()
{
  using mdt::sql::copier::CsvFileImportTableMapping;
  using mdt::sql::copier::TableMapping;

  mdt::sql::copier::CsvFileImportTableMappingDialog dialog;
  auto mapping = std::make_shared<CsvFileImportTableMapping>();
  mdtCsvParserSettings csvSettings;
  QTemporaryFile csvFile;

  /*
   * Prepare CSV source file
   */
  QVERIFY(csvFile.open());
  QVERIFY(csvFile.write("Id,Name,FieldA,FieldB\n") > 0);
  QVERIFY(csvFile.write("1,Name 1,A1,B1\n") > 0);
  csvFile.close();
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping->setSourceCsvFile(csvFile, "UTF-8", csvSettings));
  QVERIFY(mapping->setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping->mappingState() == TableMapping::MappingNotSet);
  /*
   * Setup dialog
   */
  dialog.setMapping(mapping);
  /*
   * Play
   */
  dialog.exec();
}

void mdtSqlCopierTest::copyHelperTest()
{
  using mdt::sql::copier::DatabaseCopierTableMapping;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::UniqueInsertCriteria;
  using mdt::sql::copier::RelatedTableInsertExpression;
  using mdt::sql::copier::ExpressionMatchItem;
  using mdt::sql::copier::CopyHelper;
  using mdt::sql::FieldIndexList;

  RelatedTableInsertExpression rtExp;
  UniqueInsertCriteria uic;
  std::vector<ExpressionMatchItem> expMatchItems;
  DatabaseCopierTableMapping tm;
  TableMappingItem tmi;
  QSqlDatabase db = pvDatabase;
  QString expectedSql;
  mdtExpected<QString> optSql;
  QStringList expecedFieldNameList;
  QVector<QVariant> sourceRecord;
  QVector<QVariant> destinationRecord;
  QVector<QVariant> keyRecord;
  mdtExpected< QVector<QVariant> > optRecord;
  mdtExpected<bool> optBool;
//   QVector<QVariant> expectedRecord;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  QVERIFY(db.isOpen());
  /*
   * Setup rtExp:
   *  - DFI: 1 (Client_Id_FK)
   *  - Fetch from: Client2_tbl.Id_PK
   *  - Match: Client2_tbl.Name = Address_Client_view.ClientName
   */
  rtExp.clear();
  rtExp.addDestinationFieldIndex(1);
  rtExp.setDestinationRelatedTableName("Client2_tbl");
  rtExp.addDestinationRelatedFieldIndex(0);
  expMatchItems.clear();
  expMatchItems.emplace_back(5, 1);
  rtExp.setMatchItems(expMatchItems);
  QVERIFY(!rtExp.isNull());
  /*
   * Setup unique insert criteria:
   *  - Match: Address2_tbl.FieldAA = Address_Client_view.FieldAB AND Address2_tbl.FieldAB = Address_Client_view.FieldAA
   */
  uic.clear();
  expMatchItems.clear();
  expMatchItems.emplace_back(4, 3);
  expMatchItems.emplace_back(mdtSqlWhereOperator::And, 3, 4);
  uic.setMatchItems(expMatchItems);
  QVERIFY(!uic.isNull());
  /*
   * Create table mapping:
   * ------------------------------------------------
   * |        Source field      | Destination field |
   * | (in Address_Client_view) | (in Address2_tbl) |
   * |--------------------------|-------------------|
   * |           --             | 0 (Id_PK)         |
   * |--------------------------|-------------------|
   * | rtExp                    | 1 (Client_Id_FK)  |
   * |--------------------------|-------------------|
   * | 2 (Street)               | 2 (Street)        |
   * |--------------------------|-------------------|
   * | Fixed                    | 3 (FieldAA)       |
   * |--------------------------|-------------------|
   * | 3 (FieldAA)              | 4 (FieldAB)       |
   * |----------------------------------------------|
   */
  tm.setSourceDatabase(db);
  QVERIFY(tm.setSourceTable("Address_Client_view"));
  QVERIFY(tm.setDestinationTable("Address2_tbl", db));
  tm.setUniqueInsertCriteria(uic);
  QCOMPARE(tm.itemsCount(), 5);
  tmi.setRelatedTableInsertExpression(rtExp);
  tm.insertItem(tmi);
  tm.setSourceFieldAtItem(2, "Street");
  tm.setSourceFixedValueAtItem(3, "Fixed");
  tm.setSourceFieldAtItem(4, "FieldAA");
  QCOMPARE(tm.itemsCount(), 5);
  QVERIFY(tm.itemAt(0).isNull());
  QVERIFY(!tm.itemAt(1).isNull());
  QVERIFY(!tm.itemAt(2).isNull());
  QCOMPARE(tm.itemAt(2).sourceFieldIndexList().count(), 1);
  QCOMPARE(tm.itemAt(2).sourceFieldIndexList().at(0), 2);
  QVERIFY(!tm.itemAt(3).isNull());
  QVERIFY(!tm.itemAt(4).isNull());
  QCOMPARE(tm.itemAt(4).sourceFieldIndexList().count(), 1);
  QCOMPARE(tm.itemAt(4).sourceFieldIndexList().at(0), 3);
  /*
   * Check getting SQL to count records in source table
   */
  expectedSql = "SELECT COUNT(*) FROM \"Address_Client_view\"";
  QCOMPARE(CopyHelper::getSourceTableCountSql(&tm, db), expectedSql);
  /*
   * Check getting SQL for source table select
   */
  expectedSql = "SELECT \"Id_PK\",\"Client_Id_FK\",\"Street\",\"FieldAA\",\"FieldAB\",\"ClientName\",\"ClientFieldA\",\"ClientFieldB\" " \
                "FROM \"Address_Client_view\"";
  QCOMPARE(CopyHelper::getSourceTableSelectSql(&tm, db), expectedSql);
  /*
   * Check getting SQL to count records for unique insert criteria
   */
  expectedSql = "SELECT COUNT(*) FROM \"Address2_tbl\" WHERE \"FieldAA\"=? AND \"FieldAB\"=?";
  QCOMPARE(CopyHelper::getUniqueInsertCriteriaSql(&tm, uic.matchItems(), db), expectedSql);
  /*
   * Check getting destination field name lists
   */
  // Get all fields in table mapping
  auto fieldNameLists = CopyHelper::getDestinationFieldNameLists(&tm);
  // Check related table insert expression part
  expecedFieldNameList.clear();
  expecedFieldNameList << "Client_Id_FK";
  QCOMPARE(fieldNameLists.rtiExpFieldNameList, expecedFieldNameList);
  // Check field mapping part
  expecedFieldNameList.clear();
  expecedFieldNameList << "Street" << "FieldAB";
  QCOMPARE(fieldNameLists.fieldMapFieldNameList, expecedFieldNameList);
  // Check fixed value part
  expecedFieldNameList.clear();
  expecedFieldNameList << "FieldAA";
  QCOMPARE(fieldNameLists.fixedValueFieldNameList, expecedFieldNameList);
  /*
   * Check getting SQL for insertion into destination table
   */
  expectedSql  = "INSERT INTO \"Address2_tbl\"";
  expectedSql += " (\"Client_Id_FK\",\"Street\",\"FieldAB\",\"FieldAA\")";
  expectedSql += " VALUES (?,?,?,?)";
  QCOMPARE(CopyHelper::getDestinationInsertSql(&tm, db), expectedSql);
  /*
   * Populate database for next tests
   *
   * Address_Client_view:
   * -------------------------------------
   * | Id_PK | Client_Id_FK | ClientName |
   * |-------|--------------|------------|
   * |  11   |      1       | Name 1     |
   * |-------|--------------|------------|
   * |  12   |      1       | Name 1     |
   * |-------|--------------|------------|
   * |  21   |      2       | Name 2     |
   * |-------|--------------|------------|
   *
   * Client2_tbl:
   * ------------------
   * | Id_PK |  Name  |
   * |-------|--------|
   * |   10  | Name 1 |
   * |-------|--------|
   * |   20  | Name 2 |
   * |-------|--------|
   */
  // Populate Client_tbl
  clientTableTestDataSet clientDataSet(db);
  QVERIFY(clientDataSet.populate());
  // Populate Address_tbl
  addressTableTestDataSet addressDataSet(db);
  QVERIFY(addressDataSet.populate());
  // Populate Client2_tbl
  QSqlQuery query(db);
  QVERIFY(query.exec("INSERT INTO Client2_tbl (Id_PK, Name) VALUES (10, 'Name 1')"));
  QVERIFY(query.exec("INSERT INTO Client2_tbl (Id_PK, Name) VALUES (20, 'Name 2')"));
  /*
   * Check getting related table insert expression
   */
  // Get SQL to get destination related key
  optSql = CopyHelper::getSqlForRelatedTableInsertExpressionKey(rtExp, db);
  QVERIFY(optSql);
  expectedSql = "SELECT \"Id_PK\" FROM \"Client2_tbl\" WHERE \"Name\"=?";
  QCOMPARE(optSql.value(), expectedSql);
  // Check also with more than 1 match item
  auto rtExp2 = rtExp;
  expMatchItems = rtExp.matchItems();
  expMatchItems.emplace_back(mdtSqlWhereOperator::And, 2, 2);
  rtExp2.setMatchItems(expMatchItems);
  optSql = CopyHelper::getSqlForRelatedTableInsertExpressionKey(rtExp2, db);
  QVERIFY(optSql);
  expectedSql = "SELECT \"Id_PK\" FROM \"Client2_tbl\" WHERE \"Name\"=? AND \"FieldA\"=?";
  QCOMPARE(optSql.value(), expectedSql);
  /*
   * Check getting destination record
   */
  // Build source record
  sourceRecord.clear();
  sourceRecord << 11 << 1 << "Street 11" << "FieldAA 11" << "FieldAB 11" << "Name 1" << "FieldA 1" << "FieldB 1";
  // Check getting key of related table isert expression
  optRecord = CopyHelper::getRelatedTableInsertExpressionKey(sourceRecord, rtExp, db);
  QVERIFY(optRecord);
  keyRecord = optRecord.value();
  QCOMPARE(keyRecord.size(), 1);
  QCOMPARE(keyRecord.at(0), QVariant(10));
  // Check
  ///destinationRecord = CopyHelper::getDestinationRecord(sourceRecord, &tm);
  optRecord = CopyHelper::getDestinationRecord(sourceRecord, &tm, db);
  QVERIFY(optRecord);
  destinationRecord = optRecord.value();
  QCOMPARE(destinationRecord.size(), 4);
  QCOMPARE(destinationRecord.at(0), QVariant(10));
  QCOMPARE(destinationRecord.at(1), QVariant("Street 11"));
  QCOMPARE(destinationRecord.at(2), QVariant("FieldAA 11"));
  QCOMPARE(destinationRecord.at(3), QVariant("Fixed"));
  /*
   * Checking if record exists in destination table
   */
  // Populate destination table
  QVERIFY(query.exec("INSERT INTO Address2_tbl (Id_PK, Client_Id_FK, Street, FieldAA, FieldAB) VALUES (10, 1, 'Street 10', 'FieldAA 10', 'FieldAB 10')"));
  QVERIFY(query.exec("INSERT INTO Address2_tbl (Id_PK, Client_Id_FK, Street, FieldAA, FieldAB) VALUES (11, 1, 'Street 11', 'FieldAA 11', 'FieldAB 11')"));
  // Build source record that matches
  sourceRecord.clear();
  sourceRecord << 100 << 1 << "Street 10" << "FieldAB 10" << "FieldAA 10" << "Name 1" << "FieldA 1" << "FieldB 1";
  // Check
  optBool = CopyHelper::checkExistsInDestinationTable(sourceRecord, &tm, db);
  QVERIFY(optBool);
  QVERIFY(optBool.value() == true);
  // Build source record that does not match
  sourceRecord.clear();
  sourceRecord << 100 << 1 << "Street 10" << "FieldAB 20" << "FieldAA 20" << "Name 1" << "FieldA 1" << "FieldB 1";
  // Check
  optBool = CopyHelper::checkExistsInDestinationTable(sourceRecord, &tm, db);
  QVERIFY(optBool);
  QVERIFY(optBool.value() == false);
  /*
   * Cleanup Client2_tbl and Address2_tbl
   */
  QVERIFY(query.exec("DELETE FROM Client2_tbl"));
  QVERIFY(query.exec("DELETE FROM Address2_tbl"));
}

void mdtSqlCopierTest::databaseMappingTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::DatabaseCopierTableMapping;

  mdt::sql::copier::DatabaseMapping mapping;
  std::shared_ptr<DatabaseCopierTableMapping> tm;
  auto db = pvDatabase;

  QVERIFY(db.isOpen());
  /*
   * Initial state
   */
  QCOMPARE(mapping.tableMappingCount(), 0);
  /*
   * Set destination database
   */
  QVERIFY(mapping.setDestinationDatabase(db));
  // Check attributes without any mapping set
  QCOMPARE(mapping.tableMappingCount(), 4);
  // Note: tables are sorted, and '_' is after '2' in ascii
  QCOMPARE(mapping.destinationTableName(0), QString("Address2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Address_tbl"));
  QCOMPARE(mapping.destinationTableName(2), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(3), QString("Client_tbl"));
  // Check that database is set to each table mapping
  QCOMPARE(mapping.tableMapping(0)->destinationDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(1)->destinationDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(2)->destinationDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(3)->destinationDatabase().databaseName(), db.databaseName());
  // No source must be assigned now
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  QVERIFY(mapping.sourceTableName(2).isEmpty());
  QVERIFY(mapping.sourceTableName(3).isEmpty());
  /*
   * Set source database
   */
  QVERIFY(mapping.setSourceDatabase(db));
  // Check attributes without any mapping set
  QCOMPARE(mapping.tableMappingCount(), 4);
  // Note: tables are sorted, and '_' is after '2' in ascii
  QCOMPARE(mapping.destinationTableName(0), QString("Address2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Address_tbl"));
  QCOMPARE(mapping.destinationTableName(2), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(3), QString("Client_tbl"));
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  QVERIFY(mapping.sourceTableName(2).isEmpty());
  QVERIFY(mapping.sourceTableName(3).isEmpty());
  // Check that source database was set to each table mapping
  QCOMPARE(mapping.tableMapping(0)->sourceDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(1)->sourceDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(2)->sourceDatabase().databaseName(), db.databaseName());
  QCOMPARE(mapping.tableMapping(3)->sourceDatabase().databaseName(), db.databaseName());
  /*
   * Edit table mapping:
   *  Table Client_tbl -> Client2_tbl
   *  Fields:
   *   Client_tbl.Id_PK -> Client2_tbl.Id_PK
   *   Client_tbl.Name -> Client2_tbl.Name
   *   Client_tbl.FieldB -> Client2_tbl.FieldA
   *   Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  tm = mapping.tableMapping(2);
  QVERIFY(tm->sourceDatabase().isOpen());
  QVERIFY(tm->setSourceTable("Client_tbl"));
  tm->setSourceFieldAtItem(0, "Id_PK");
  tm->setSourceFieldAtItem(1, "Name");
  tm->setSourceFieldAtItem(2, "FieldB");
  tm->setSourceFieldAtItem(3, "FieldA");
  QVERIFY(tm->mappingState() == TableMapping::MappingComplete);
  ///mapping.setTableMapping(0, tm);
  QCOMPARE(mapping.tableMappingCount(), 4);
  QCOMPARE(mapping.sourceTableName(2), QString("Client_tbl"));
  QCOMPARE(mapping.destinationTableName(2), QString("Client2_tbl"));
  /*
   * Reset mapping
   */
  QVERIFY(mapping.resetMapping());
  QCOMPARE(mapping.tableMappingCount(), 4);
  // Check destinations tables
  QCOMPARE(mapping.destinationTableName(0), QString("Address2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Address_tbl"));
  QCOMPARE(mapping.destinationTableName(2), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(3), QString("Client_tbl"));
  // Check source tables
  QVERIFY(mapping.sourceTableName(0).isEmpty());
  QVERIFY(mapping.sourceTableName(1).isEmpty());
  QVERIFY(mapping.sourceTableName(2).isEmpty());
  QVERIFY(mapping.sourceTableName(3).isEmpty());
  /*
   * Generate mapping by names
   */
  QVERIFY(mapping.generateTableMappingByName());
  QCOMPARE(mapping.tableMappingCount(), 4);
  // Check destinations tables
  QCOMPARE(mapping.destinationTableName(0), QString("Address2_tbl"));
  QCOMPARE(mapping.destinationTableName(1), QString("Address_tbl"));
  QCOMPARE(mapping.destinationTableName(2), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableName(3), QString("Client_tbl"));
  // Check source tables
  QCOMPARE(mapping.sourceTableName(0), QString("Address2_tbl"));
  QCOMPARE(mapping.sourceTableName(1), QString("Address_tbl"));
  QCOMPARE(mapping.sourceTableName(2), QString("Client2_tbl"));
  QCOMPARE(mapping.sourceTableName(3), QString("Client_tbl"));
  /*
   * Clear mapping
   */
  mapping.clearMapping();
  QCOMPARE(mapping.tableMappingCount(), 0);
//   QVERIFY(!mapping.sourceDatabase().isValid());
//   QVERIFY(!mapping.destinationDatabase().isValid());
}

void mdtSqlCopierTest::databaseMappingModelBaseTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::DatabaseCopierTableMapping;
  using mdt::sql::copier::DatabaseMapping;
  using mdt::sql::copier::MappingModel;
  using mdt::sql::copier::DatabaseMappingModel;

  QTableView tableView;
  QTreeView treeView;
  DatabaseMappingModel model;
  const int selectedColumn = 0;
  const int copyProgressColumn = 4;
  const int copyStatusColumn = 5;
  QModelIndex index;
  auto db = pvDatabase;

  QVERIFY(db.isOpen());
  /*
   * Initial state
   */
  QCOMPARE(model.columnCount(), 6);
  QCOMPARE(model.rowCount(), 0);
  /*
   * Generate mapping
   */
  QVERIFY(model.setSourceDatabase(db));
  QVERIFY(model.setDestinationDatabase(db));
  QCOMPARE(model.rowCount(), 4);
  QVERIFY(model.generateTableMappingByName());
  QCOMPARE(model.rowCount(), 4);
  /*
   * Check initial copy progress and status
   */
  // Check progress of row 0
  index = model.index(0, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 1
  index = model.index(1, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 2
  index = model.index(2, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 3
  index = model.index(3, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check status of row 0
  index = model.index(0, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 1
  index = model.index(1, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 2
  index = model.index(2, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 3
  index = model.index(3, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  /*
   * Copy progress update
   */
  // Update progress of row 1 and 2
  model.setTableCopyProgress(1, 10);
  model.setTableCopyProgress(2, 20);
  // Check progress of row 0
  index = model.index(0, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 1
  index = model.index(1, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(10));
  // Check progress of row 2
  index = model.index(2, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(20));
  // Check progress of row 3
  index = model.index(3, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  /*
   * Copy status update
   */
  // Update status of row 0 and 2
  model.setTableCopyStatus(0, MappingModel::CopyStatusOk);
  model.setTableCopyStatus(2, MappingModel::CopyStatusError);
  // Check status of row 0
  index = model.index(0, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::green)));
  // Check status of row 1
  index = model.index(1, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 2
  index = model.index(2, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::red)));
  // Check status of row 3
  index = model.index(3, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  /*
   * Clear copy status and progress
   */
  model.clearCopyStatusAndProgress();
  // Check progress of row 0
  index = model.index(0, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 1
  index = model.index(1, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 2
  index = model.index(2, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check progress of row 3
  index = model.index(3, copyProgressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check status of row 0
  index = model.index(0, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 1
  index = model.index(1, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 2
  index = model.index(2, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  // Check status of row 3
  index = model.index(3, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());
  /*
   * Set copy error
   */
  // Set error to row 1
  model.setTableCopyError(1, mdtErrorNewQ("Error 1", mdtError::Error, this));
  // Check status of row 1
  index = model.index(1, copyStatusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::red)));
  /*
   * Check table mapping selection
   */
  // Check if row 0 is selected
  index = model.index(0, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Unchecked));
  QVERIFY(!model.isTableMappingSelected(0));
  // Check if row 1 is selected
  index = model.index(1, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Unchecked));
  QVERIFY(!model.isTableMappingSelected(1));
  // Check if row 2 is selected
  index = model.index(2, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Unchecked));
  QVERIFY(!model.isTableMappingSelected(2));
  // Check if row 3 is selected
  index = model.index(3, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Unchecked));
  QVERIFY(!model.isTableMappingSelected(3));
  /*
   * Check setting all complete table mapping selected for copy
   */
  model.setAllCompleteTableMappingSelected();
  // Check if row 0 is selected
  index = model.index(0, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Checked));
  QVERIFY(model.isTableMappingSelected(0));
  // Check if row 1 is selected
  index = model.index(1, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Checked));
  QVERIFY(model.isTableMappingSelected(1));
  // Check if row 2 is selected
  index = model.index(2, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Checked));
  QVERIFY(model.isTableMappingSelected(2));
  // Check if row 3 is selected
  index = model.index(3, selectedColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::CheckStateRole), QVariant(Qt::Checked));
  QVERIFY(model.isTableMappingSelected(3));

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  mdtProgressBarItemDelegate *progressDelegate = new mdtProgressBarItemDelegate(&tableView);
  tableView.setItemDelegateForColumn(copyProgressColumn, progressDelegate);
  tableView.resize(600, 150);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::databaseMappingModelTest()
{
  using mdt::sql::copier::TableMapping;
  using mdt::sql::copier::DatabaseCopierTableMapping;
  using mdt::sql::copier::DatabaseMapping;
  using mdt::sql::copier::DatabaseMappingModel;

  QTableView tableView;
  QTreeView treeView;
  DatabaseMappingModel model;
  const int sourceTableNameColumn = 0;
  const int destinationTableNameColumn = 1;
  const int tableMappingStateColumn = 2;
  QModelIndex index;
  std::shared_ptr<DatabaseCopierTableMapping> tm;
  auto db = pvDatabase;

  QVERIFY(db.isOpen());
  /*
   * Initial state
   */
  QCOMPARE(model.columnCount(), 5);
  QCOMPARE(model.rowCount(), 0);
  /*
   * Check generating mapping by name
   */
  // Set databases and generate by name
  QVERIFY(model.setSourceDatabase(db));
  QVERIFY(model.setDestinationDatabase(db));
  QCOMPARE(model.rowCount(), 4);
  QVERIFY(model.generateTableMappingByName());
  QCOMPARE(model.rowCount(), 4);
  // Check row 0
  index = model.index(0, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Address2_tbl"));
  index = model.index(0, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Address2_tbl"));
  index = model.index(0, tableMappingStateColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::green)));
  // Check row 1
  index = model.index(1, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Address_tbl"));
  index = model.index(1, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Address_tbl"));
  index = model.index(1, tableMappingStateColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::green)));
  // Check row 2
  index = model.index(2, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client2_tbl"));
  index = model.index(2, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client2_tbl"));
  index = model.index(2, tableMappingStateColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::green)));
  // Check row 3
  index = model.index(3, sourceTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl"));
  index = model.index(3, destinationTableNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl"));
  index = model.index(3, tableMappingStateColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant(QColor(Qt::green)));
  /*
   * Check updating table mapping
   */
  tm = model.tableMapping(0);
  QVERIFY(tm.get() != nullptr);
  tm->resetFieldMapping();
  model.tableMappingUpdated(0);
  QCOMPARE(model.rowCount(), 4);
  // Check row 0
  index = model.index(0, tableMappingStateColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DecorationRole), QVariant());

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.resize(600, 150);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Play
   */
  tableView.resizeColumnsToContents();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtSqlCopierTest::databaseCopyThreadTest()
{
  using mdt::sql::copier::DatabaseCopierTableMapping;
  using mdt::sql::copier::DatabaseMapping;
  using mdt::sql::copier::DatabaseMappingModel;
  using mdt::sql::copier::DatabaseCopyThread;

  DatabaseCopyThread thread;
  DatabaseMappingModel model;
  
//   DatabaseMapping mapping;
  
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
  QVERIFY(model.setSourceDatabase(db));
  QVERIFY(model.setDestinationDatabase(db));
  QCOMPARE(model.rowCount(), 4);
  
//   QVERIFY(mapping.setSourceDatabase(db));
//   QVERIFY(mapping.setDestinationDatabase(db));
//   QCOMPARE(mapping.tableMappingCount(), 4); 
  /*
   * Edit table mapping:
   *  Table Client_tbl -> Client2_tbl
   *  Fields:
   *   Client_tbl.Id_PK -> Client2_tbl.Id_PK
   *   'Fixed name' -> Client2_tbl.Name
   *   Client_tbl.FieldB -> Client2_tbl.FieldA
   *   Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  auto tm = model.tableMapping(2);
//   auto tm = mapping.tableMapping(2);
  QCOMPARE(tm->destinationTableName(), QString("Client2_tbl"));
  QVERIFY(tm->sourceDatabase().isOpen());
  QVERIFY(tm->setSourceTable("Client_tbl"));
  tm->setSourceFieldAtItem(0, "Id_PK");
  tm->setSourceFixedValueAtItem(1, "Fixed name");
  tm->setSourceFieldAtItem(2, "FieldB");
  tm->setSourceFieldAtItem(3, "FieldA");
  QVERIFY(tm->mappingState() == DatabaseCopierTableMapping::MappingComplete);
  model.tableMappingUpdated(2);
  QCOMPARE(model.rowCount(), 4);
  model.setAllCompleteTableMappingSelected();
//   QCOMPARE(mapping.tableMappingCount(), 4);
  /*
   * Run copy
   */
  thread.startCopy(&model);
//   thread.startCopy(mapping);
  thread.wait();
  /*
   * Check that copy was done
   */
  QVERIFY(query.exec("SELECT * FROM Client2_tbl"));
  // Row for Id_PK = 1
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("Fixed name"));
  QCOMPARE(query.value(2), QVariant("FieldB 1"));
  QCOMPARE(query.value(3), QVariant("FieldA 1"));
  // Row for Id_PK = 2
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("Fixed name"));
  QCOMPARE(query.value(2), QVariant("FieldB 2"));
  QCOMPARE(query.value(3), QVariant("FieldA 2"));
  /*
   * Cleanup
   */
  QVERIFY(query.exec("DELETE FROM Client2_tbl"));
}

void mdtSqlCopierTest::databaseCopyDialogTest()
{
  mdt::sql::copier::DatabaseCopyDialog dialog;
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
  dialog.exec();
}

/*
 * Helper functions
 */

void mdtSqlCopierTest::beginEditing(QAbstractItemView& view, const QModelIndex& index)
{
  QVERIFY(view.editTriggers() & QAbstractItemView::DoubleClicked);
  QVERIFY(index.isValid());

  // Get view port (witch is the widget to witch event must be sent)
  QWidget *viewPort = view.viewport();
  QVERIFY(viewPort != nullptr);
  // Get center of region in view port that concerns given index
  QRect rect = view.visualRect(index);
  QPoint itemCenter = rect.center();
  // Edition beginns after double click. With QTest, we must click before
  QTest::mouseClick(viewPort, Qt::LeftButton, 0, itemCenter);
  QTest::mouseDClick(viewPort, Qt::LeftButton, 0, itemCenter);
}

void mdtSqlCopierTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex)
{
  QVERIFY(!(view.editTriggers() & QAbstractItemView::SelectedClicked));
  QVERIFY(editingIndex.isValid());
  QVERIFY(view.model() != nullptr);
  QVERIFY(view.model()->rowCount() > 1);

  // Select a other row than witch we are editing
  int row = editingIndex.row();
  if(row == 0){
    ++row;
  }else{
    --row;
  }
  QModelIndex index = view.model()->index(row, editingIndex.column());
  // Get view port (witch is the widget to witch event must be sent)
  QWidget *viewPort = view.viewport();
  QVERIFY(viewPort != nullptr);
  // Get center of region in view port that concerns given index
  QRect rect = view.visualRect(index);
  QPoint itemCenter = rect.center();
  // Now, click to select new row
  QTest::mouseClick(viewPort, Qt::LeftButton, 0, itemCenter);
}


/*
 * Test data base manipulation methods
 */

void mdtSqlCopierTest::createTestDatabase()
{
  using mdtSqlViewSchema::SelectField;

  mdtSqlTableSchema table;
  mdtSqlDatabaseSchema s;
  mdtSqlField field;
  mdtSqlField Client_Id_FK;
  mdtSqlForeignKey fk_Client_Id_FK;

  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());

  /*
   * Create Client_tbl
   */
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
  /*
   * Create Address_tbl
   */
  table.clear();
  table.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Client_Id_FK
  Client_Id_FK.clear();
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(mdtSqlFieldType::Integer);
  Client_Id_FK.setRequired(true);
  table.addField(Client_Id_FK, false);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(150);
  table.addField(field, false);
  // FieldAA
  field.clear();
  field.setName("FieldAA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldAB
  field.clear();
  field.setName("FieldAB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // fk_Client_Id_FK
  fk_Client_Id_FK.clear();
  fk_Client_Id_FK.setParentTableName("Client_tbl");
  fk_Client_Id_FK.addKeyFields("Id_PK", Client_Id_FK);
  table.addForeignKey(fk_Client_Id_FK);
  s.addTable(table);
  /*
   * Create Address_Client_view
   */
  mdtSqlViewSchema::Schema ADR_CLI_view;
  mdtSqlViewSchema::Table CLI("Client_tbl", "CLI");
  mdtSqlViewSchema::Table ADR("Address_tbl", "ADR");
  mdtSqlViewSchema::JoinClause ADR_CLI_join;
  ADR_CLI_view.setName("Address_Client_view");
  ADR_CLI_view.setTable(ADR);
  ADR_CLI_view.addSelectField(ADR, SelectField("Id_PK"));
  ADR_CLI_view.addSelectField(ADR, SelectField("Client_Id_FK"));
  ADR_CLI_view.addSelectField(ADR, SelectField("Street"));
  ADR_CLI_view.addSelectField(ADR, SelectField("FieldAA"));
  ADR_CLI_view.addSelectField(ADR, SelectField("FieldAB"));
  ADR_CLI_view.addSelectField(CLI, SelectField("Name", "ClientName"));
  ADR_CLI_view.addSelectField(CLI, SelectField("FieldA", "ClientFieldA"));
  ADR_CLI_view.addSelectField(CLI, SelectField("FieldB", "ClientFieldB"));
  ADR_CLI_join = s.joinClause(ADR, CLI);
  QVERIFY(!ADR_CLI_join.isNull());
  ADR_CLI_view.addJoinClause(ADR_CLI_join);
  s.addView(ADR_CLI_view);
  /*
   * Create Client2_tbl
   */
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
  /*
   * Create Address2_tbl
   */
  table.clear();
  table.setTableName("Address2_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  table.addField(field, true);
  // Client_Id_FK
  Client_Id_FK.clear();
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(mdtSqlFieldType::Integer);
  Client_Id_FK.setRequired(true);
  table.addField(Client_Id_FK, false);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(150);
  table.addField(field, false);
  // FieldAA
  field.clear();
  field.setName("FieldAA");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // FieldAB
  field.clear();
  field.setName("FieldAB");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // fk_Client_Id_FK
  fk_Client_Id_FK.clear();
  fk_Client_Id_FK.setParentTableName("Client2_tbl");
  fk_Client_Id_FK.addKeyFields("Id_PK", Client_Id_FK);
  table.addForeignKey(fk_Client_Id_FK);
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
