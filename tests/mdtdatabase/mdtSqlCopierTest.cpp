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
#include "mdtSqlCopierDataMapping.h"

#include "mdt/sql/copier/SourceField.h"
#include "mdt/sql/copier/SourceFieldIndex.h"
#include "mdt/sql/copier/SourceFixedValue.h"
#include "mdt/sql/copier/SourceFieldExpression.h"

#include "mdt/sql/copier/FieldIndexList.h"
#include "mdt/sql/copier/TableMappingItem.h"
#include "mdt/sql/copier/FieldMapping.h"
#include "mdt/sql/copier/FixedValue.h"
#include "mdt/sql/copier/UniqueInsertExpression.h"
#include "mdt/sql/copier/TableMappingEditHelper.h"

#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtSqlDatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierThread.h"
#include "mdtSqlDatabaseCopierMapping.h"
#include "mdtSqlDatabaseCopierMappingModel.h"
#include "mdtSqlDatabaseCopierDialog.h"
#include "mdtSqlCsvStringImportTableMapping.h"
#include "mdtSqlCsvFileImportTableMapping.h"
#include "mdtSqlCsvStringImportTableMappingModel.h"
#include "mdtSqlCsvFileImportTableMappingModel.h"
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

void mdtSqlCopierTest::sandbox2()
{
  using mdt::sql::copier::SourceFieldExpression;
  using mdt::sql::copier::SourceFieldExpressionMatchItem;

  SourceFieldExpression exp;

  exp.setTablesForUniqueInsert("SomeSourceTable", "Client_tbl");
  exp.addMatchItemForUniqueInsert("SrcName", "Name");
  exp.addMatchItemForUniqueInsert(SourceFieldExpressionMatchItem::And, "rem", "REM");
  
  qDebug() << "SQL: " << exp.getSql(pvDatabase, "Id_PK");
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

void mdtSqlCopierTest::sourceFieldIndexTest()
{
  using mdt::sql::copier::SourceFieldIndex;
  using mdt::sql::copier::AbstractSourceField;

  std::unique_ptr<AbstractSourceField> asf;

  /*
   * Constructions
   */
  SourceFieldIndex index1;
  QCOMPARE(index1.fieldIndex(), -1);
  QVERIFY(index1.isNull());
  /*
   * Simple set/get
   */
  index1.setFieldIndex(2);
  QCOMPARE(index1.fieldIndex(), 2);
  QVERIFY(!index1.isNull());
  // Also check using base class pointer
  asf.reset(new SourceFieldIndex);
  QVERIFY(asf->isNull());
  asf->setFieldIndex(3);
  QCOMPARE(asf->fieldIndex(), 3);
  QVERIFY(!asf->isNull());
  /*
   * Clear
   */
  QVERIFY(asf.get() != nullptr);
  // Set all members
  asf->setFieldIndex(5);
  QCOMPARE(asf->fieldIndex(), 5);
  QVERIFY(!asf->isNull());
  // Clear and check
  asf->clear();
  QCOMPARE(asf->fieldIndex(), -1);
  QVERIFY(asf->isNull());

}

void mdtSqlCopierTest::sourceFixedValueTest()
{
  using mdt::sql::copier::SourceFixedValue;
  using mdt::sql::copier::AbstractSourceField;

  std::unique_ptr<AbstractSourceField> asf;

  /*
   * Constructions
   */
  asf.reset(new SourceFixedValue);
  QVERIFY(asf->fixedValue().isNull());
  QVERIFY(asf->isNull());
  /*
   * Simple set/get
   */
  asf->setFixedValue("Fixed value");
  QVERIFY(!asf->isNull());
  QCOMPARE(asf->fixedValue(), QVariant("Fixed value"));
  /*
   * Clear
   */
  asf->clear();
  QVERIFY(asf->fixedValue().isNull());
  QVERIFY(asf->isNull());
}

void mdtSqlCopierTest::sourceFieldExpressionTest()
{

}

void mdtSqlCopierTest::sourceFieldTest()
{
  using mdt::sql::copier::SourceField;

  /*
   * Constructions of field indexes
   */
  SourceField sourceFieldIndex1(SourceField::SourceFieldIndexType);
  QVERIFY(sourceFieldIndex1.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sourceFieldIndex1.fieldIndex(), -1);
  QVERIFY(sourceFieldIndex1.isNull());
  /*
   * Copy construction of field indexes
   */
  // Set some values to sourceFieldIndex1
  sourceFieldIndex1.setFieldIndex(1);
  QCOMPARE(sourceFieldIndex1.fieldIndex(), 1);
  // Copy construct sourceFieldIndex2 and check
  SourceField sourceFieldIndex2(sourceFieldIndex1);
  QCOMPARE(sourceFieldIndex2.fieldIndex(), 1);
  // Now, update sourceFieldIndex2 (will detach and clone) and check
  sourceFieldIndex2.setFieldIndex(2);
  QCOMPARE(sourceFieldIndex1.fieldIndex(), 1);
  QCOMPARE(sourceFieldIndex2.fieldIndex(), 2);
  /*
   * Copy assignement of field indexes
   */
  // Create sourceFieldIndex3 and set it (prevent the compiler to make copy construct)
  SourceField sourceFieldIndex3(SourceField::SourceFieldIndexType);
  sourceFieldIndex3.setFieldIndex(3);
  QCOMPARE(sourceFieldIndex3.fieldIndex(), 3);
  // Assign
  sourceFieldIndex3 = sourceFieldIndex1;
  QCOMPARE(sourceFieldIndex1.fieldIndex(), 1);
  QCOMPARE(sourceFieldIndex3.fieldIndex(), 1);
  // Update sourceFieldIndex3 and check
  sourceFieldIndex3.setFieldIndex(3);
  QCOMPARE(sourceFieldIndex1.fieldIndex(), 1);
  QCOMPARE(sourceFieldIndex2.fieldIndex(), 2);
  QCOMPARE(sourceFieldIndex3.fieldIndex(), 3);
  /*
   * Construction of fixed values
   */
  SourceField fixedValue1(SourceField::SourceFixedValueType);
  QVERIFY(fixedValue1.type() == SourceField::SourceFixedValueType);
  QVERIFY(fixedValue1.isNull());
  /*
   * Copy construction of fixed values
   */
  // Set some value to fixedValue1
  fixedValue1.setFixedValue("value 1");
  QCOMPARE(fixedValue1.fixedValue(), QVariant("value 1"));
  // Create fixedValue2 as copy of fixedValue1
  SourceField fixedValue2(fixedValue1);
  QCOMPARE(fixedValue2.fixedValue(), QVariant("value 1"));
  // Update fixedValue2
  fixedValue2.setFixedValue("value 2");
  QCOMPARE(fixedValue1.fixedValue(), QVariant("value 1"));
  QCOMPARE(fixedValue2.fixedValue(), QVariant("value 2"));
  /*
   * Copy assignemnt of fixed values
   */
  // Create fixedValue3
  SourceField fixedValue3(SourceField::SourceFixedValueType);
  // Assign
  fixedValue3 = fixedValue1;
  QCOMPARE(fixedValue3.fixedValue(), QVariant("value 1"));
  // Update fixedValue3
  fixedValue3.setFixedValue("value 3");
  QCOMPARE(fixedValue1.fixedValue(), QVariant("value 1"));
  QCOMPARE(fixedValue2.fixedValue(), QVariant("value 2"));
  QCOMPARE(fixedValue3.fixedValue(), QVariant("value 3"));
  /*
   * Copy construction of different types
   */
  // Set sf1 as copy of sourceFieldIndex1
  SourceField sf1(sourceFieldIndex1);
  QVERIFY(sf1.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sf1.fieldIndex(), 1);
  // Update sf1
  sf1.setFieldIndex(11);
  QCOMPARE(sf1.fieldIndex(), 11);
  QCOMPARE(sourceFieldIndex1.fieldIndex(), 1);
  // Set sf2 as copy of fixedValue2
  SourceField sf2(fixedValue2);
  QVERIFY(sf2.type() == SourceField::SourceFixedValueType);
  QCOMPARE(sf2.fixedValue(), QVariant("value 2"));
  // Update sf2
  sf2.setFixedValue("value 22");
  QCOMPARE(sf2.fixedValue(), QVariant("value 22"));
  QCOMPARE(fixedValue2.fixedValue(), QVariant("value 2"));
  /*
   * Copy assignment of different types
   */
  sf2 = sf1;
  QVERIFY(sf2.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sf2.fieldIndex(), 11);
  // Update sf1
  sf1.setFieldIndex(111);
  QVERIFY(sf1.type() == SourceField::SourceFieldIndexType);
  QVERIFY(sf2.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sf1.fieldIndex(), 111);
  QCOMPARE(sf2.fieldIndex(), 11);
  // Update sf2
  sf2.setFieldIndex(1111);
  QVERIFY(sf1.type() == SourceField::SourceFieldIndexType);
  QVERIFY(sf2.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sf1.fieldIndex(), 111);
  QCOMPARE(sf2.fieldIndex(), 1111);
  /*
   * Check setting different type of one existing SourceField object works
   */
  SourceField sf(SourceField::SourceFieldIndexType);
  // Set a field index
  sf.setFieldIndex(5);
  QVERIFY(sf.type() == SourceField::SourceFieldIndexType);
  QCOMPARE(sf.fieldIndex(), 5);
  // Set a fixed value
  sf.setFixedValue("value 5");
  QVERIFY(sf.type() == SourceField::SourceFixedValueType);
  QCOMPARE(sf.fixedValue(), QVariant("value 5"));
  /*
   * Construction of expression
   */
  SourceField expression1(SourceField::SourceFieldExpressionType);
  QVERIFY(expression1.type() == SourceField::SourceFieldExpressionType);
  QVERIFY(expression1.isNull());
  /*
   * Copy construction of expression
   */
  /// \todo Implement
  // Set some value to expression1
  
  // Construct expression2 as copy of expression1
  SourceField expression2(expression1);
  
  // Update expression2
  
  /*
   * Copy assignmenet
   */
  /// \todo Implement
  // Create expression3
  SourceField expression3(SourceField::SourceFieldExpressionType);
  // Assign
  expression3 = expression1;
  
  // Update expression3
  

}

void mdtSqlCopierTest::fieldMappingDataTest()
{
  using mdt::sql::copier::SourceField;
  mdtSqlCopierFieldMapping data;

  /*
   * Initial state
   */
  QVERIFY(data.sourceField.type() == SourceField::SourceFieldIndexType);
  QVERIFY(data.sourceField.isNull());
  ///QCOMPARE(data.sourceFieldIndex, -1);
  QCOMPARE(data.destinationFieldIndex, -1);
  QVERIFY(data.mappingState == mdtSqlCopierFieldMapping::MappingNotSet);
  ///QVERIFY(data.sourceType == mdtSqlCopierFieldMapping::Field);
  QVERIFY(data.isNull());
  /*
   * Set with Field source type
   */
  // Check when simply set source and destination field index
  ///data.sourceFieldIndex = 0;
  data.sourceField.setFieldIndex(0);
  QVERIFY(data.isNull());
  data.destinationFieldIndex = 0;
  QVERIFY(!data.isNull());
  // Set some other members to check clear
  data.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
  ///data.sourceType = mdtSqlCopierFieldMapping::FixedValue;
  ///data.sourceFixedValue = "Fixed";
  /*
   * Clear
   */
  data.clear();
  ///QCOMPARE(data.sourceFieldIndex, -1);
  QVERIFY(data.sourceField.isNull());
  QCOMPARE(data.destinationFieldIndex, -1);
  QVERIFY(data.mappingState == mdtSqlCopierFieldMapping::MappingNotSet);
  ///QVERIFY(data.sourceType == mdtSqlCopierFieldMapping::Field);
  ///QVERIFY(data.sourceFixedValue.isNull());
  QVERIFY(data.isNull());
  /*
   * Set with FixedValue source type
   */
  ///data.sourceType = mdtSqlCopierFieldMapping::FixedValue;
  // Set normally
  QVERIFY(data.isNull());
  data.sourceField.setFixedValue("Fixed value");
  ///data.sourceFixedValue = "Fixed value";
  QVERIFY(data.isNull());
  data.destinationFieldIndex = 0;
  QVERIFY(!data.isNull());
  // Set a valid source index (must be ignored)
//   data.clear();
//   ///data.sourceType = mdtSqlCopierFieldMapping::FixedValue;
//   QVERIFY(data.isNull());
//   data.destinationFieldIndex = 0;
//   QVERIFY(data.isNull());
//   data.sou
//   data.sourceFieldIndex = 0;
//   QVERIFY(data.isNull());
//   data.sourceFixedValue = "Fixed value";
//   QVERIFY(!data.isNull());
}

void mdtSqlCopierTest::fieldIndexListTest()
{
  using mdt::sql::copier::FieldIndexList;

  FieldIndexList dfIndexList;
  FieldIndexList a, b;

  /*
   * Initial state
   */
  QCOMPARE(dfIndexList.count(), 0);
  QVERIFY(dfIndexList.isEmpty());
  /*
   * Set / get
   */
  // Add a field index
  dfIndexList.append(10);
  QCOMPARE(dfIndexList.count(), 1);
  QVERIFY(!dfIndexList.isEmpty());
  // Getters
  QCOMPARE(dfIndexList.at(0), 10);
  // Check iterating
  for(const auto i : dfIndexList){
    QCOMPARE(i, 10);
  }
  // Check assignation of a vector
  dfIndexList = {11, 12};
  QCOMPARE(dfIndexList.count(), 2);
  QCOMPARE(dfIndexList.at(0), 11);
  QCOMPARE(dfIndexList.at(1), 12);
  /*
   * Clear
   */
  dfIndexList.clear();
  QCOMPARE(dfIndexList.count(), 0);
  QVERIFY(dfIndexList.isEmpty());
  /*
   * Comparisons
   */
  // a == b with 1 field index
  a = {0};
  b = {0};
  QVERIFY(a == b);
  QVERIFY(!(a < b));
  QVERIFY(a <= b);
  // a < b with 1 field index
  a = {0};
  b = {1};
  QVERIFY(!(a == b));
  QVERIFY(a < b);
  QVERIFY(a <= b);
  // a > b with 1 field index
  a = {1};
  b = {0};
  QVERIFY(!(a == b));
  QVERIFY(!(a < b));
  QVERIFY(!(a <= b));

}

void mdtSqlCopierTest::fieldMappingTest()
{
  using mdt::sql::copier::FieldMapping;
  using mdt::sql::copier::TableMappingItemState;

  /*
   * Initial state
   */
  FieldMapping fm;
  QCOMPARE(fm.sourceFieldIndex(), -1);
  QCOMPARE(fm.destinationFieldIndexList().count(), 0);
  QVERIFY(fm.mappingState() == TableMappingItemState::MappingNotSet);
  QVERIFY(fm.isNull());
  /*
   * Set
   */
  fm.setFieldMapping(1, 2);
  QCOMPARE(fm.sourceFieldIndex(), 1);
  QCOMPARE(fm.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm.destinationFieldIndexList().at(0), 2);
  QVERIFY(!fm.isNull());
  fm.setMappingState(TableMappingItemState::MappingComplete);
  QVERIFY(fm.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Clear
   */
  fm.clear();
  QCOMPARE(fm.sourceFieldIndex(), -1);
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

void mdtSqlCopierTest::uniqueInsertExpressionTest()
{
  using mdt::sql::copier::UniqueInsertExpression;
  using mdt::sql::copier::UniqueInsertMatchExpressionItem;
  using mdt::sql::copier::TableMappingItemState;

  /*
   * Match item tests
   */
  // Constructs
  UniqueInsertMatchExpressionItem item1(-1, -1);
  QVERIFY(item1.isNull());
  UniqueInsertMatchExpressionItem item2(0, -1);
  QVERIFY(item2.isNull());
  UniqueInsertMatchExpressionItem item3(-1, 0);
  QVERIFY(item3.isNull());
  // Construct a not null item
  UniqueInsertMatchExpressionItem item4(1, 2);
  QCOMPARE(item4.sourceValueFieldIndex, 1);
  QCOMPARE(item4.destinationFieldIndex, 2);
  QVERIFY(item4.operatorWithPrevious == mdtSqlWhereOperator::Null);
  QVERIFY(!item4.isNull());
  // Construct a non null item with operator
  UniqueInsertMatchExpressionItem item5(mdtSqlWhereOperator::And, 3, 4);
  QCOMPARE(item5.sourceValueFieldIndex, 3);
  QCOMPARE(item5.destinationFieldIndex, 4);
  QVERIFY(item5.operatorWithPrevious == mdtSqlWhereOperator::And);
  QVERIFY(!item5.isNull());
  // Clear
  ///item5.cl
  /*
   * Initial state
   */
  UniqueInsertExpression exp;
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(exp.destinationFieldIndexList().count(), 0);
  QCOMPARE(exp.getSourceValueFieldIndexList().size(), 0);
  QVERIFY(exp.isNull());
  /*
   * Setup expression
   */
  exp.clear();
  // Set destination field indexes
  exp.addDestinationFieldIndex(1);
  QCOMPARE(exp.destinationFieldIndexList().count(), 1);
  QCOMPARE(exp.destinationFieldIndexList().at(0), 1);
  QCOMPARE(exp.getSourceValueFieldIndexList().size(), 0);
  QVERIFY(exp.isNull());
  // Add a match item
  exp.addMatchItem(3, 4);
  QCOMPARE(exp.getSourceValueFieldIndexList().size(), 1);
  QCOMPARE(exp.getSourceValueFieldIndexList().at(0), 3);
  QVERIFY(!exp.isNull());
  // Add a match item
  exp.addMatchItem(5, 6);
  QCOMPARE(exp.getSourceValueFieldIndexList().size(), 2);
  QCOMPARE(exp.getSourceValueFieldIndexList().at(0), 3);
  QCOMPARE(exp.getSourceValueFieldIndexList().at(1), 5);
  QVERIFY(!exp.isNull());
  // Set mapping state
  exp.setMappingState(TableMappingItemState::MappingComplete);
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Copy construction
   */
  UniqueInsertExpression exp2(exp);
  QCOMPARE(exp2.destinationFieldIndexList().count(), 1);
  QCOMPARE(exp2.destinationFieldIndexList().at(0), 1);
  QCOMPARE(exp2.getSourceValueFieldIndexList().size(), 2);
  QCOMPARE(exp2.getSourceValueFieldIndexList().at(0), 3);
  QCOMPARE(exp2.getSourceValueFieldIndexList().at(1), 5);
  QVERIFY(!exp2.isNull());
  QVERIFY(exp2.mappingState() == TableMappingItemState::MappingComplete);
  /*
   * Copy assignment
   */
  // Create exp3 and set it (make shure compiler don't use copy constructor)
//   UniqueInsertExpression exp3;
//   exp3.addDestinationFieldIndex(6);
//   QCOMPARE(exp3.destinationFieldIndexList().count(), 1);
//   // Assign
//   exp3 = exp;

  /*
   * Clear
   */
  // Be shure we have something to clear
  QVERIFY(exp.destinationFieldIndexList().count() > 0);
  QVERIFY(exp.getSourceValueFieldIndexList().size() > 0);
  QVERIFY(exp.mappingState() != TableMappingItemState::MappingNotSet);
  // Clear
  exp.clear();
  QCOMPARE(exp.destinationFieldIndexList().count(), 0);
  QCOMPARE(exp.getSourceValueFieldIndexList().size(), 0);
  QVERIFY(exp.mappingState() == TableMappingItemState::MappingNotSet);
}

void mdtSqlCopierTest::tableMappingItemTest()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::TableMappingItemState;
  using mdt::sql::copier::UniqueInsertExpression;

  /*
   * Construction of field mapping item
   */
  TableMappingItem fm1(TableMappingItem::FieldMappingType);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndex(), -1);
  QVERIFY(fm1.mappingState() == TableMappingItemState::MappingNotSet);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 0);
  QVERIFY(fm1.isNull());
  /*
   * Copy construction of field mapping item
   */
  // Set some value to fm1
  fm1.setFieldMapping(1, 11);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
//   QCOMPARE(fm1.destinationFieldIndex(), 11);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QVERIFY(!fm1.isNull());
  // Construct fm2 on base of fm1
  TableMappingItem fm2(fm1);
  QVERIFY(fm2.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm2.sourceFieldIndex(), 1);
//   QCOMPARE(fm2.destinationFieldIndex(), 11);
  QCOMPARE(fm2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm2.destinationFieldIndexList().at(0), 11);
  QVERIFY(!fm2.isNull());
  // Update fm2
  fm2.setFieldMapping(2, 22);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
//   QCOMPARE(fm1.destinationFieldIndex(), 11);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QCOMPARE(fm2.sourceFieldIndex(), 2);
//   QCOMPARE(fm2.destinationFieldIndex(), 22);
  QCOMPARE(fm2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm2.destinationFieldIndexList().at(0), 22);
  /*
   * Copy assignment of field mapping item
   */
  // Create fm3
  TableMappingItem fm3(TableMappingItem::FieldMappingType);
  QVERIFY(fm3.isNull());
  // Assign
  fm3 = fm1;
  QVERIFY(fm3.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm3.sourceFieldIndex(), 1);
//   QCOMPARE(fm3.destinationFieldIndex(), 11);
  QCOMPARE(fm3.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm3.destinationFieldIndexList().at(0), 11);
  // Update fm3
  fm3.setFieldMapping(3, 33);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
//   QCOMPARE(fm1.destinationFieldIndex(), 11);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QCOMPARE(fm3.sourceFieldIndex(), 3);
//   QCOMPARE(fm3.destinationFieldIndex(), 33);
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
  QCOMPARE(fm4.sourceFieldIndex(), 1);
//   QCOMPARE(fm4.destinationFieldIndex(), 11);
  QCOMPARE(fm4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm4.destinationFieldIndexList().at(0), 11);
  // Update fm4
  fm4.setFieldMapping(4, 44);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
//   QCOMPARE(fm1.destinationFieldIndex(), 11);
  QCOMPARE(fm1.destinationFieldIndexList().count(), 1);
  QCOMPARE(fm1.destinationFieldIndexList().at(0), 11);
  QCOMPARE(fm4.sourceFieldIndex(), 4);
//   QCOMPARE(fm4.destinationFieldIndex(), 44);
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
  QCOMPARE(fv4.sourceFieldIndex(), 1);
  QCOMPARE(fv4.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv4.destinationFieldIndexList().at(0), 11);
  // Set fixed value to fv4
  fv4.setFixedValue("Fixed 4", 4);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
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
  QCOMPARE(fv2.sourceFieldIndex(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().count(), 1);
  QCOMPARE(fv2.destinationFieldIndexList().at(0), 11);
  // Set fixed value to fv2
  fv2.setFixedValue("Fixed 22", 22);
  QVERIFY(fm1.type() == TableMappingItem::FieldMappingType);
  QCOMPARE(fm1.sourceFieldIndex(), 1);
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
  UniqueInsertExpression uniqueInsertExpression;
  uniqueInsertExpression.addDestinationFieldIndex(1);
  uniqueInsertExpression.addDestinationFieldIndex(2);
  uniqueInsertExpression.addMatchItem(3, 4);
  uniqueInsertExpression.addMatchItem(mdtSqlWhereOperator::And, 5, 6);
  // Create table mapping item
  TableMappingItem uiexp(TableMappingItem::UniqueInsertExpressionType);
  QVERIFY(uiexp.type() == TableMappingItem::UniqueInsertExpressionType);
  QVERIFY(uiexp.isNull());
  // Set expression
  uiexp.setUniqueInsertExpression(uniqueInsertExpression);
  QVERIFY(uiexp.type() == TableMappingItem::UniqueInsertExpressionType);
  QCOMPARE(uiexp.destinationFieldIndexList().count(), 2);
  QCOMPARE(uiexp.sourceFieldIndex(), -1);
  QCOMPARE(uiexp.destinationFieldIndexList().at(0), 1);
  QCOMPARE(uiexp.destinationFieldIndexList().at(1), 2);
  QVERIFY(!uiexp.isNull());
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

void mdtSqlCopierTest::tableMappingEditHelperContainsDFIdexesTest()
{
  using mdt::sql::copier::TableMappingEditHelper;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::FieldIndexList;

  QFETCH(TableMappingItem, item);
  QFETCH(FieldIndexList, fieldIndexList);
  QFETCH(bool, expectedMatch);

  QVERIFY(TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList) == expectedMatch);
}

void mdtSqlCopierTest::tableMappingEditHelperContainsDFIdexesTest_data()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::FieldIndexList;

  QTest::addColumn<TableMappingItem>("item");
  QTest::addColumn<FieldIndexList>("fieldIndexList");
  QTest::addColumn<bool>("expectedMatch");

  TableMappingItem item;
  FieldIndexList fieldIndexList;

  /*
   * Empty item
   */
  fieldIndexList.clear();
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {0};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {1};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {2};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {0,1};
  QTest::newRow("") << item << fieldIndexList << false;
  /*
   * Field mapping item
   */
  item.setFieldMapping(1, 2);
  fieldIndexList.clear();
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {0};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {1};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {2};
  QTest::newRow("") << item << fieldIndexList << true;
  fieldIndexList = {0,1};
  QTest::newRow("") << item << fieldIndexList << false;
  fieldIndexList = {0,1,2};
  QTest::newRow("") << item << fieldIndexList << true;
}

void mdtSqlCopierTest::tableMappingEditHelperItemIdexByDFIndexesTest()
{
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::FieldIndexList;

}

void mdtSqlCopierTest::tableMappingEditHelperItemIdexByDFIndexesTest_data()
{

}

void mdtSqlCopierTest::tableMappingEditHelperTest()
{
  using mdt::sql::copier::TableMappingEditHelper;
  using mdt::sql::copier::TableMappingItem;
  using mdt::sql::copier::UniqueInsertExpression;
  using mdt::sql::copier::FieldIndexList;

  QVector<TableMappingItem> allItems;
  QVector<TableMappingItem> itemsList;
  QVector<int> itemsIndexList;
  TableMappingItem item(TableMappingItem::FieldMappingType);
  UniqueInsertExpression exp;
  FieldIndexList fieldIndexList;

  /*
   * Checking if item contains destinations fields test
   */
  // Check with empty item
//   item.clear();
//   QCOMPARE(item.destinationFieldIndexList().count(), 0);
//   fieldIndexList = {0};
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList = {2};
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList.clear();
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   // Setup a field mapping
//   item.setFieldMapping(1, 2);
//   fieldIndexList = {0};
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList = {2};
//   QVERIFY(TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList = {0,3};
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList = {0,1,2};
//   QVERIFY(TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));
//   fieldIndexList.clear();
//   QVERIFY(!TableMappingEditHelper::itemContainsDestinationFieldIndex(item, fieldIndexList));

  /*
   * Build a list of TableMappingItem:
   * -----------------------------
   * | Item | Src fld | Dest fld |
   * -----------------------------
   * |   0  |    1    |    0     |
   * -----------------------------
   * |   1  |    0    |    1     |
   * -----------------------------
   * |   2  |    2    |    2     |
   * -----------------------------
   */
  allItems.clear();
  // Index 0 : field mapping
  item.setFieldMapping(1, 0);
  allItems << item;
  // Index 1 : field mapping
  item.setFieldMapping(0, 1);
  allItems << item;
  // Index 2 : field mapping
  item.setFieldMapping(2, 2);
  allItems << item;
  QCOMPARE(allItems.size(), 3);
  /*
   * Edit item at index 1:
   *  Type: expression
   *  Source field index: -
   *  Destination field indexes: 0,1
   */
  // Setup expression
  exp.clear();
  exp.addDestinationFieldIndex(0);
  exp.addDestinationFieldIndex(1);
  item.setUniqueInsertExpression(exp);
  /*
   * Checks
   */
  // Indexes of items to remove: 0
  itemsIndexList = TableMappingEditHelper::getItemsToRemoveIndexList(item, 1, allItems);
  QCOMPARE(itemsIndexList.size(), 1);
  QCOMPARE(itemsIndexList.at(0), 0);
  // Items to add: -
  itemsList = TableMappingEditHelper::getItemsToAddList(item, 1, allItems);
  QCOMPARE(itemsList.size(), 0);
//   itemsIndexList = TableMappingEditHelper::getItemsToAddIndexList(item, 1, allItems);
//   QCOMPARE(itemsIndexList.size(), 0);
  /*
   * Now we have following table mapping:
   * -----------------------------
   * | Item | Src fld | Dest fld |
   * -----------------------------
   * |   0  |    -    |    0,1   |
   * -----------------------------
   * |   1  |    2    |    2     |
   * -----------------------------
   */
  allItems.clear();
  // Index 0 : expression
  item.setUniqueInsertExpression(exp);
  QCOMPARE(item.destinationFieldIndexList().count(), 2);
  allItems << item;
  // Index 1 : field mapping
  item.setFieldMapping(2, 2);
  allItems << item;
  QCOMPARE(allItems.size(), 2);
  /*
   * Check getting index of item to insert reagrding its destination field indexes
   */
  ///QCOMPARE(TableMappingEditHelper::getIndexOfItemToInsertByDFIndexes({}, allItems) , 0);
  fieldIndexList = {0};
  QCOMPARE(TableMappingEditHelper::getIndexOfItemToInsertByDFIndexes(fieldIndexList, allItems) , 1);
  fieldIndexList = {1};
  QCOMPARE(TableMappingEditHelper::getIndexOfItemToInsertByDFIndexes(fieldIndexList, allItems) , 1);
  fieldIndexList = {2};
  QCOMPARE(TableMappingEditHelper::getIndexOfItemToInsertByDFIndexes(fieldIndexList, allItems) , 2);
  fieldIndexList = {3};
  QCOMPARE(TableMappingEditHelper::getIndexOfItemToInsertByDFIndexes(fieldIndexList, allItems) , 2);
  /*
   * Edit item at index 0:
   *  Type: field mapping
   *  Source field index: 1
   *  Destination field index: 0
   */
  item.setFieldMapping(1, 0);
  /*
   * Checks
   */
  // Indexes of items to remove: none
  itemsIndexList = TableMappingEditHelper::getItemsToRemoveIndexList(item, 0, allItems);
  QCOMPARE(itemsIndexList.size(), 0);
  // Items to add: -
  /*
   * Items to add:
   * ----------------------
   * | Src fld | Dest fld |
   * ----------------------
   * |         |    1     |
   * ----------------------
   */
  itemsList = TableMappingEditHelper::getItemsToAddList(item, 0, allItems);
  QCOMPARE(itemsList.size(), 1);
  QCOMPARE(itemsList.at(0).destinationFieldIndexList().count(), 1);
  QCOMPARE(itemsList.at(0).destinationFieldIndexList().at(0), 1);
  // Index of items to add: 1
//   itemsIndexList = TableMappingEditHelper::getItemsToAddIndexList(item, 0, allItems);
//   QCOMPARE(itemsIndexList.size(), 1);
//   QCOMPARE(itemsIndexList.at(0), 1);
  /*
   * Update table mapping, witch conducts to:
   * -----------------------------
   * | Item | Src fld | Dest fld |
   * -----------------------------
   * |   0  |    1    |    0     |
   * -----------------------------
   * |   1  |         |    1     |
   * -----------------------------
   * |   2  |    2    |    2     |
   * -----------------------------
   */
  // Update table mapping
  /// \todo implement

}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingTest()
{
  using mdt::sql::copier::SourceField;
  using mdt::sql::copier::TableMapping;

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
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QString("Client_tbl"));
  QCOMPARE(mapping.sourceTableFieldCount(), 4);
  QCOMPARE(mapping.sourceTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.sourceTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.sourceTableFieldNameAt(3), QString("FieldB"));
  // Check for destination table
  QCOMPARE(mapping.destinationTableName(), QString("Client2_tbl"));
  QCOMPARE(mapping.destinationTableFieldCount(), 4);
  QCOMPARE(mapping.destinationTableFieldNameAt(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationTableFieldNameAt(1), QString("Name"));
  QCOMPARE(mapping.destinationTableFieldNameAt(2), QString("FieldA"));
  QCOMPARE(mapping.destinationTableFieldNameAt(3), QString("FieldB"));
  /*
   * Check attributes without any mapping set
   */
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingNotSet);
  // Check field key types
  QVERIFY(mapping.sourceFieldKeyType(0) == TableMapping::NotAKey);
  QVERIFY(mapping.destinationFieldKeyType(0) == TableMapping::PrimaryKey);
  QVERIFY(mapping.sourceFieldKeyType(1) == TableMapping::NotAKey);
  QVERIFY(mapping.destinationFieldKeyType(1) == TableMapping::NotAKey);
  // Check that default source types are set
  QVERIFY(mapping.sourceType(0) == SourceField::SourceFieldIndexType);
  QVERIFY(mapping.sourceType(1) == SourceField::SourceFieldIndexType);
  QVERIFY(mapping.sourceType(2) == SourceField::SourceFieldIndexType);
  QVERIFY(mapping.sourceType(3) == SourceField::SourceFieldIndexType);
//   QVERIFY(mapping.sourceType(0) == mdtSqlCopierFieldMapping::Field);
//   QVERIFY(mapping.sourceType(1) == mdtSqlCopierFieldMapping::Field);
//   QVERIFY(mapping.sourceType(2) == mdtSqlCopierFieldMapping::Field);
//   QVERIFY(mapping.sourceType(3) == mdtSqlCopierFieldMapping::Field);
  /*
   * Set a field mapping:
   *  - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id_PK");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingNotSet);
  // Check field key types
  QVERIFY(mapping.sourceFieldKeyType(0) == TableMapping::PrimaryKey);
  QVERIFY(mapping.destinationFieldKeyType(0) == TableMapping::PrimaryKey);
  QVERIFY(mapping.sourceFieldKeyType(1) == TableMapping::NotAKey);
  QVERIFY(mapping.destinationFieldKeyType(1) == TableMapping::NotAKey);
  /*
   * Set a field mapping:
   *  - Client_tbl.Name -> Client2_tbl.Name
   */
  mapping.setSourceField(1, "Name");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingNotSet);
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldB -> Client2_tbl.FieldA
   */
  mapping.setSourceField(2, "FieldB");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingNotSet);
  /*
   * Set a field mapping:
   *  - Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  mapping.setSourceField(3, "FieldA");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingComplete);
  /*
   * Reset
   */
  mapping.resetFieldMapping();
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingNotSet);
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldA"));
  QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldB"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(2) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(3) == mdtSqlCopierFieldMapping::MappingComplete);
  /*
   * Check field mapping with fixed value:
   *  - Map source field Id_PK -> destination field Id_PK
   *  - Map a fixed value -> destination Name
   */
  // Reset
  mapping.resetFieldMapping();
  // Check state after reset
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingNotSet);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingNotSet);
  // Check that default source types are set
  QVERIFY(mapping.sourceType(0) == SourceField::SourceFieldIndexType);
  QVERIFY(mapping.sourceType(1) == SourceField::SourceFieldIndexType);
//   QVERIFY(mapping.sourceType(0) == mdtSqlCopierFieldMapping::Field);
//   QVERIFY(mapping.sourceType(1) == mdtSqlCopierFieldMapping::Field);
  /*
   * Update source type for Name destination field
   */
//   mapping.setSourceType(1, mdtSqlCopierFieldMapping::FixedValue);
//   QVERIFY(mapping.sourceType(1) == mdtSqlCopierFieldMapping::FixedValue);
  /*
   * Map Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id_PK");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
//   QVERIFY(mapping.sourceFixedValue(1).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingNotSet);
  /*
   * Set a fixed value for Client2_tbl.Name
   */
  mapping.setSourceFixedValue(1, "Fixed name");
  // Check mapping
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id_PK"));
  QCOMPARE(mapping.sourceFixedValue(1), QVariant("Fixed name"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  QVERIFY(mapping.fieldMappingState(1) == mdtSqlCopierFieldMapping::MappingComplete);
  // Check also that source type are set
  QVERIFY(mapping.sourceType(0) == SourceField::SourceFieldIndexType);
  QVERIFY(mapping.sourceType(1) == SourceField::SourceFixedValueType);
  /*
   * Check field mapping state after changing source type
   */
  // Reset
  mapping.resetFieldMapping();
  // Check state after reset
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  // Check field mapping state
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingNotSet);
  // Check that default source types are set
//   QVERIFY(mapping.sourceType(0) == mdtSqlCopierFieldMapping::Field);
  QVERIFY(mapping.sourceType(0) == SourceField::SourceFieldIndexType);
  // Set a valid field mapping
  mapping.setSourceField(0, "Id_PK");
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);
  // Change source type
//   mapping.setSourceType(0, mdtSqlCopierFieldMapping::FixedValue);
//   QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingNotSet);
  // Set a valid value
  mapping.setSourceFixedValue(0, 5);
  QVERIFY(mapping.fieldMappingState(0) == mdtSqlCopierFieldMapping::MappingComplete);

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
  QVERIFY(mapping.setSourceTable("Client_tbl", db));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", db));
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
  /*
   * Check with source fixed value for Client2_tbl.Name
   */
//   mapping.setSourceType(1, mdtSqlCopierFieldMapping::FixedValue);
  mapping.setSourceFixedValue(1, "Fixed name");
  // Check SQL select data in source table
  expectedSql = "SELECT \"Id_PK\",\"FieldB\",\"FieldA\" FROM \"Client_tbl\"";
  QCOMPARE(mapping.getSqlForSourceTableSelect(db), expectedSql);
  // Check SQL to insert into destination table
  expectedSql = "INSERT INTO \"Client2_tbl\" (\"Id_PK\",\"Name\",\"FieldA\",\"FieldB\") VALUES (?,'Fixed name',?,?)";
  QCOMPARE(mapping.getSqlForDestinationTableInsert(db), expectedSql);

}

void mdtSqlCopierTest::sqlDatabaseCopierTableMappingModelTest()
{
  using mdt::sql::copier::SourceField;

  QTableView tableView;
  QTreeView treeView;
  mdtSqlDatabaseCopierTableMappingModel model;
  const int sourceTypeColumn = 0;
  const int sourceFieldNameColumn = 2;
  const int destinationFieldNameColumn = 6;
  QModelIndex index;
//   mdtSqlCopierFieldMapping::SourceType sourceType;
  SourceField::Type sourceType;
  mdtSqlDatabaseCopierTableMapping tm;
  mdtComboBoxItemDelegate *sourceTypeDelegate = new mdtComboBoxItemDelegate(&tableView);
  mdtComboBoxItemDelegate *sourceFieldNameDelegate = new mdtComboBoxItemDelegate(&tableView);

  /*
   * Setup views
   */
  // Setup table view
  tableView.setModel(&model);
  tableView.setItemDelegateForColumn(sourceTypeColumn, sourceTypeDelegate);
  tableView.setItemDelegateForColumn(sourceFieldNameColumn, sourceFieldNameDelegate);
  tableView.resize(800, 200);
  tableView.show();
  // Setup tree view
  treeView.setModel(&model);
  treeView.show();

  /*
   * Check by generating by name
   */
  // Set tables and generate field mapping
  model.setupSourceTypeDelegate(sourceTypeDelegate);
  QVERIFY(model.setSourceTable("Client_tbl", pvDatabase, sourceFieldNameDelegate));
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
  sourceType = static_cast<SourceField::Type>(model.data(index, Qt::EditRole).toInt());
  QVERIFY(sourceType == SourceField::SourceFixedValueType);
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
  
  qDebug() << tm.getSourceFieldNameList();
  
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

void mdtSqlCopierTest::sqlCopierDataMappingTest()
{
  mdtSqlDatabaseCopierTableMapping mapping;
  mdtSqlCopierDataMapping dataMapping;
  QSqlDatabase db = pvDatabase;
  QSqlQuery sourceQuery(db);
  QString sql;

  /*
   * Populate Client_tbl
   */
  clientTableTestDataSet dataSet(db);
  QVERIFY(dataSet.populate());
  /*
   * Setup databases and tables
   */
  QVERIFY(mapping.setSourceTable("Client_tbl", db));
  QVERIFY(mapping.setDestinationTable("Client2_tbl", db));
  /*
   * Add field mapping:
   * - Client_tbl.Id_PK -> Client2_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id_PK");
  // Get source data
  sql = mapping.getSqlForSourceTableSelect(db);
  QVERIFY(sourceQuery.exec(sql));
  QVERIFY(sourceQuery.next());
  QCOMPARE(sourceQuery.record().count(), 1);
  // Get destination data
  dataMapping.setSourceRecord(sourceQuery.record(), mapping);
  QCOMPARE(dataMapping.size(), 1);
  QCOMPARE(dataMapping.destinationRecord.at(0), QVariant(1));
  /*
   * Add field mapping:
   * - Client_tbl.Name -> Client2_tbl.Name
   */
  mapping.setSourceField(1, "Name");
  // Get source data
  sql = mapping.getSqlForSourceTableSelect(db);
  QVERIFY(sourceQuery.exec(sql));
  QVERIFY(sourceQuery.next());
  QCOMPARE(sourceQuery.record().count(), 2);
  // Get destination data
  dataMapping.setSourceRecord(sourceQuery.record(), mapping);
  QCOMPARE(dataMapping.size(), 2);
  QCOMPARE(dataMapping.destinationRecord.at(0), QVariant(1));
  QCOMPARE(dataMapping.destinationRecord.at(1), QVariant("Name 1"));
  /*
   * Add field mapping:
   * - Client_tbl.FieldB -> Client2_tbl.FieldA
   */
  mapping.setSourceField(2, "FieldB");
  // Get source data
  sql = mapping.getSqlForSourceTableSelect(db);
  QVERIFY(sourceQuery.exec(sql));
  QVERIFY(sourceQuery.next());
  QCOMPARE(sourceQuery.record().count(), 3);
  // Get destination data
  dataMapping.setSourceRecord(sourceQuery.record(), mapping);
  QCOMPARE(dataMapping.size(), 3);
  QCOMPARE(dataMapping.destinationRecord.at(0), QVariant(1));
  QCOMPARE(dataMapping.destinationRecord.at(1), QVariant("Name 1"));
  QCOMPARE(dataMapping.destinationRecord.at(2), QVariant("FieldB 1"));
  /*
   * Add field mapping:
   * - Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  mapping.setSourceField(3, "FieldA");
  // Get source data
  sql = mapping.getSqlForSourceTableSelect(db);
  QVERIFY(sourceQuery.exec(sql));
  QVERIFY(sourceQuery.next());
  QCOMPARE(sourceQuery.record().count(), 4);
  // Get destination data
  dataMapping.setSourceRecord(sourceQuery.record(), mapping);
  QCOMPARE(dataMapping.size(), 4);
  QCOMPARE(dataMapping.destinationRecord.at(0), QVariant(1));
  QCOMPARE(dataMapping.destinationRecord.at(1), QVariant("Name 1"));
  QCOMPARE(dataMapping.destinationRecord.at(2), QVariant("FieldB 1"));
  QCOMPARE(dataMapping.destinationRecord.at(3), QVariant("FieldA 1"));
  /*
   * Check with source fixed value for Client2_tbl.Name
   */
//   mapping.setSourceType(1, mdtSqlCopierFieldMapping::FixedValue);
  mapping.setSourceFixedValue(1, "Fixed name");
  // Get source data
  sql = mapping.getSqlForSourceTableSelect(db);
  QVERIFY(sourceQuery.exec(sql));
  QVERIFY(sourceQuery.next());
  QCOMPARE(sourceQuery.record().count(), 3);
  // Get destination data
  dataMapping.setSourceRecord(sourceQuery.record(), mapping);
  QCOMPARE(dataMapping.size(), 4);
  QCOMPARE(dataMapping.destinationRecord.at(0), QVariant(1));
  QCOMPARE(dataMapping.destinationRecord.at(1), QVariant("Fixe name"));
  QCOMPARE(dataMapping.destinationRecord.at(2), QVariant("FieldB 1"));
  QCOMPARE(dataMapping.destinationRecord.at(3), QVariant("FieldA 1"));
}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingTest()
{
  mdtSqlCsvStringImportTableMapping mapping;
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
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvString(csvString, csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Check fetching tables specific informations
   */
  // Check for source table
  QCOMPARE(mapping.sourceTableName(), QString("CSV String")); /// \todo Define what name it should be
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
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check also source and destination type names
  QVERIFY(mapping.sourceFieldTypeName(0).isNull());
  QCOMPARE(mapping.destinationFieldTypeName(0), QString("INTEGER"));
  // Check getting source field names
  fieldNames = mapping.getSourceFieldNameList();
  QCOMPARE(fieldNames.size(), 4);
  QCOMPARE(fieldNames.at(0), QString("Id"));
  QCOMPARE(fieldNames.at(1), QString("Name"));
  QCOMPARE(fieldNames.at(2), QString("FieldA"));
  QCOMPARE(fieldNames.at(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.Id -> Client_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Reset
   */
  mapping.resetFieldMapping();
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  /*
   * Check field mapping generation by field name
   */
  mapping.generateFieldMappingByName();
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldA"));
  QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldB"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Now, manually map CSV source Id -> Client_tbl Id_PK
  mapping.setSourceField(0, "Id");
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  /*
   * Clear
   */
  mapping.clearFieldMapping();
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);

}

void mdtSqlCopierTest::sqlCsvStringImportTableMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlCsvStringImportTableMapping tm;
  mdtSqlCsvStringImportTableMappingModel model;
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
  QVERIFY(model.setSourceCsvString(csvString, csvSettings, delegate));
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

void mdtSqlCopierTest::sqlCsvFileImportTableMappingTest()
{
  mdtSqlCsvFileImportTableMapping mapping;
  mdtCsvParserSettings csvSettings;
  QTemporaryFile csvFile;
  QStringList fieldNames;

  /*
   * Prepare CSV source file
   */
  QVERIFY(csvFile.open());
  QVERIFY(csvFile.write("Id,Name,FieldA,FieldB\n") > 0);
  QVERIFY(csvFile.write("1,Name 1,A1,B1\n") > 0);
  csvFile.close();
  /*
   * Initial state
   */
  QCOMPARE(mapping.fieldCount(), 0);
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
  /*
   * Set source CSV and destination table
   */
  QVERIFY(mapping.setSourceCsvFile(csvFile, "UTF-8", csvSettings));
  QVERIFY(mapping.setDestinationTable("Client_tbl", pvDatabase));
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingNotSet);
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
  QCOMPARE(mapping.fieldCount(), 4);
  QVERIFY(mapping.sourceFieldNameAtItem(0).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
  // Check also source and destination type names
  QVERIFY(mapping.sourceFieldTypeName(0).isNull());
  QCOMPARE(mapping.destinationFieldTypeName(0), QString("INTEGER"));
  // Check getting source field names
  fieldNames = mapping.getSourceFieldNameList();
  QCOMPARE(fieldNames.size(), 4);
  QCOMPARE(fieldNames.at(0), QString("Id"));
  QCOMPARE(fieldNames.at(1), QString("Name"));
  QCOMPARE(fieldNames.at(2), QString("FieldA"));
  QCOMPARE(fieldNames.at(3), QString("FieldB"));
  /*
   * Set a field mapping:
   *  - CSV.Id -> Client_tbl.Id_PK
   */
  mapping.setSourceField(0, "Id");
  // Check attributes
  QVERIFY(mapping.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingPartial);
  QCOMPARE(mapping.fieldCount(), 4);
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QVERIFY(mapping.sourceFieldNameAtItem(1).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QVERIFY(mapping.sourceFieldNameAtItem(2).isNull());
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QVERIFY(mapping.sourceFieldNameAtItem(3).isNull());
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
  QCOMPARE(mapping.sourceFieldNameAtItem(0), QString("Id"));
  QCOMPARE(mapping.sourceFieldNameAtItem(1), QString("Name"));
  QCOMPARE(mapping.sourceFieldNameAtItem(2), QString("FieldB"));
  QCOMPARE(mapping.sourceFieldNameAtItem(3), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(0), QString("Id_PK"));
  QCOMPARE(mapping.destinationFieldName(1), QString("Name"));
  QCOMPARE(mapping.destinationFieldName(2), QString("FieldA"));
  QCOMPARE(mapping.destinationFieldName(3), QString("FieldB"));
}

void mdtSqlCopierTest::sqlCsvFileImportTableMappingModelTest()
{
  QTableView tableView;
  QTreeView treeView;
  mdtSqlCsvFileImportTableMapping tm;
  mdtSqlCsvFileImportTableMappingModel model;
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
  QVERIFY(model.setSourceCsvFile(csvFile, "UTF-8", csvSettings, delegate));
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
  dialog.exec();
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
   *   'Fixed name' -> Client2_tbl.Name
   *   Client_tbl.FieldB -> Client2_tbl.FieldA
   *   Client_tbl.FieldA -> Client2_tbl.FieldB
   */
  auto tm = mapping.tableMapping(0);
  QVERIFY(tm.setSourceTable("Client_tbl", pvDatabase));
  tm.setSourceField(0, "Id_PK");
//   tm.setSourceType(1, mdtSqlCopierFieldMapping::FixedValue);
  tm.setSourceFixedValue(1, "Fixed name");
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
