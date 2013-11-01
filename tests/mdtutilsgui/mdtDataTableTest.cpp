/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtDataTableTest.h"
#include "mdtDataTableManager.h"
#include "mdtDataTableModel.h"
#include "mdtApplication.h"
#include "mdtDataTableItemDelegate.h"
#include "mdtFieldMap.h"
#include "mdtFieldMapItem.h"

#include "mdtFieldMapDialog.h"
#include "mdtFieldMapItemDialog.h"
#include "mdtFieldListEditionDialog.h"

#include "mdtSqlQueryWidget.h"
#include "mdtSqlSchemaTable.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QString>
#include <QStringList>
#include <QSqlTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QModelIndex>
#include <QVariant>
#include <QTimer>
#include <QScrollBar>
#include <QPushButton>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QMap>
#include <QHash>

#include <QDebug>

void mdtDataTableTest::sandbox()
{
  mdtFieldMapItemDialog dialog;
  mdtFieldMapDialog fmDialog;
  QList<mdtFieldMapField> sourceFields;
  QList<mdtFieldMapField> destinationFields;
  mdtFieldMapField field;
  mdtFieldMap fieldMap;
  mdtFieldMapItem mapItem;

  
  
  // Fill source fields
  field.setIndex(0);
  field.setName("SRC01");
  field.setDisplayText("Source 1");
  sourceFields << field;
  field.setIndex(1);
  field.setName("SRC02");
  field.setDisplayText("");
  sourceFields << field;
  field.setIndex(2);
  field.setName("SRC03");
  field.setDisplayText("Source 3");
  sourceFields << field;
  // Fill destination fields
  field.setIndex(0);
  field.setName("DEST01");
  field.setDisplayText("Destination 1");
  destinationFields << field;
  field.setIndex(1);
  field.setName("DEST02");
  field.setDisplayText("");
  destinationFields << field;
  field.setIndex(2);
  field.setName("DEST03");
  field.setDisplayText("Destination 3");
  destinationFields << field;
  field.setIndex(3);
  field.setName("DEST04");
  field.setDisplayText("Destination 4");
  destinationFields << field;

  /**
  mdtFieldListEditionDialog fldListDialog;
  fldListDialog.setAvailableFields(destinationFields);
  fldListDialog.exec();
  
  destinationFields = fldListDialog.fields();
  for(int i = 0; i < destinationFields.size(); ++i){
    field = destinationFields.at(i);
    qDebug() << "Field idx " << field.index() << ": " << field.name() << " - " << field.displayText();
  }
  
  return;
  */

  fieldMap.setSourceFields(sourceFields);
  fieldMap.setDestinationFields(destinationFields);
  fmDialog.setFieldMap(fieldMap);
  fmDialog.exec();
  
  /**
  dialog.setMapItem(mapItem);
  dialog.setSourceFields(sourceFields);
  dialog.setDestinationFields(destinationFields);
  dialog.exec();
  
  qDebug() << "mapItem (A):";
  qDebug() << "-> source field index: " << mapItem.sourceFieldIndex();
  qDebug() << "-> source field name : " << mapItem.sourceFieldName();
  qDebug() << "-> source field data start offset : " << mapItem.sourceFieldDataStartOffset();
  qDebug() << "-> source field data end   offset : " << mapItem.sourceFieldDataEndOffset();
  qDebug() << "-> destination field index : " << mapItem.fieldIndex();
  qDebug() << "-> destination field name : " << mapItem.fieldName();
  qDebug() << "-> destination field D. text : " << mapItem.destinationFieldDisplayText();

  mapItem = dialog.mapItem();
  qDebug() << "mapItem (B):";
  qDebug() << "-> source field index: " << mapItem.sourceFieldIndex();
  qDebug() << "-> source field name : " << mapItem.sourceFieldName();
  qDebug() << "-> source field data start offset : " << mapItem.sourceFieldDataStartOffset();
  qDebug() << "-> source field data end   offset : " << mapItem.sourceFieldDataEndOffset();
  qDebug() << "-> destination field index : " << mapItem.fieldIndex();
  qDebug() << "-> destination field name : " << mapItem.fieldName();
  qDebug() << "-> destination field D. text : " << mapItem.destinationFieldDisplayText();
  */

  QSKIP("Just a sanbox test", SkipAll);

  QSqlDatabase db;
  QString sql;
  double value;

  randomValueInit();

  db = QSqlDatabase::addDatabase("QSQLITE", "mdtDataTableTest");
  db.setDatabaseName("mdtdatatabletest.db");
  ///db.setDatabaseName("/tmp/mdtdatatabletest-l.o.db");
  QVERIFY(db.open());

  QSqlQuery query(db);

  // Create data table
  sql = "CREATE TABLE IF NOT EXISTS data ( ";
  ///sql = "CREATE TABLE data ( ";
  sql += "id_PK INTEGER PRIMARY KEY ";
  sql += ", ";
  sql += "name VARCHAR(250) ";
  sql += ", ";
  sql += "details VARCHAR(250) ";
  sql += ", ";
  sql += "value REAL ";
  sql += ", ";
  sql += "result VARCHAR(20) ";
  sql += ")";
  qDebug() << "SQL: " << sql;
  QVERIFY(query.exec(sql));

  // Model
  QSqlTableModel m(0, db);
  m.setTable("data");
  m.select();

  // View
  QTableView v;
  v.setItemDelegate(new mdtDataTableItemDelegate);
  v.setModel(&m);
  v.setEditTriggers(QAbstractItemView::NoEditTriggers);
  v.resize(600, 600);
  v.show();

  // Add data
  for(int i=0; i<10; i++){
    QVERIFY(m.insertRows(0, 1));
    QVERIFY(m.setData(m.index(0, 1), "Test " + QString::number(i)));
    QVERIFY(m.setData(m.index(0, 2), "Détails " + QString::number(i+5)));
    /**
    if(i%2){
      QVERIFY(m.setData(m.index(0, 3), "Ok"));
    }else{
      QVERIFY(m.setData(m.index(0, 3), "Nok"));
    }
    */
    ///m.setIndexWidget(m.index(0, 0), new QPushButton);
    QVERIFY(m.submit());
    if(m.rowCount() < 100){
      ///QTimer::singleShot(0, &v, SLOT(scrollToBottom()));
    }
    ///QTest::qWait(500);
  }

  // Set results
  for(int i=0; i<m.rowCount(); i++){
    ///value = 3.75*sin(i);
    value = 3.75*sin(i)*randomValue(-5, 5);
    QVERIFY(m.setData(m.index(i, 3), value));
    if(value > 0.0){
      QVERIFY(m.setData(m.index(i, 4), "Ok"));
    }else{
      QVERIFY(m.setData(m.index(i, 4), "Nok"));
    }
    QVERIFY(m.submit());
    QTest::qWait(500);
  }

  QTest::qWait(1000);

  // Enable unit checks
  for(int i=0; i<m.rowCount(); i++){
    v.setIndexWidget(m.index(i, 0), new QPushButton("Run test " + m.data(m.index(i, 0)).toString()));
  }
  
  while(v.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDataTableTest::createDataSetTest()
{
  mdtDataTableManager manager;
  mdtDataTableModel *model;
  QTemporaryFile dbFile;
  QFileInfo fileInfo;
  QSqlField field;
  QString dataSetName;
  QString dataSetTableName;
  mdtSqlSchemaTable table;

  // Build fields
  field = QSqlField();
  field.setName("id_PK");
  field.setType(QVariant::Int);
  table.addField(field, true);
  field = QSqlField();
  field.setName("value");
  field.setType(QVariant::Double);
  table.addField(field, false);
  field = QSqlField();
  field.setName("name");
  field.setType(QVariant::String);
  table.addField(field, false);

  // Check data set creation modes
  QVERIFY(dbFile.open());
  fileInfo.setFile(dbFile);
  dataSetName = fileInfo.fileName();
  dataSetTableName = manager.getTableName(dataSetName);
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(manager.database().isOpen());
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::KeepExisting));
  QVERIFY(manager.database().isOpen());
  QVERIFY(!manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::FailIfExists));
  QVERIFY(manager.database().isOpen());
  manager.close();

  // Check table creation: PK fields are automatically created
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(manager.database().isOpen());
  // Get model and check that columns exists
  model = manager.model();
  QVERIFY(model != 0);
  QCOMPARE(model->tableName(), dataSetTableName);
  QCOMPARE(model->columnCount(), 3);
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("value"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("name"));
  // Check that data set is empty
  QVERIFY(model->select());
  QCOMPARE(model->rowCount(), 0);
  manager.close();

  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(manager.database().isOpen());
  // Get model and check that columns exists
  model = manager.model();
  QVERIFY(model != 0);
  QCOMPARE(model->tableName(), dataSetTableName);
  QCOMPARE(model->columnCount(), 3);
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("value"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("name"));
  // Check that data set is empty
  QVERIFY(model->select());
  QCOMPARE(model->rowCount(), 0);
}

void mdtDataTableTest::editDataTest()
{
  mdtDataTableManager manager;
  mdtDataTableModel *model;
  QTemporaryFile dbFile;
  QFileInfo fileInfo;
  QSqlField field;
  QString dataSetName;
  QString dataSetTableName;
  QMap<QString,QVariant> rowData;
  QList<QVariant> rowData2;
  mdtFieldMap fieldMap;
  mdtFieldMapItem *mapItem;
  QStringList rowData3;
  QList<QStringList> rowsData4;
  mdtSqlSchemaTable table;

  // Build fields
  field = QSqlField();
  field.setName("id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  field = QSqlField();
  field.setName("signal");
  field.setType(QVariant::Double);
  table.addField(field, false);
  field = QSqlField();
  field.setName("value");
  field.setType(QVariant::String);
  table.addField(field, false);
  // Create data set
  QVERIFY(dbFile.open());
  fileInfo.setFile(dbFile);
  dataSetName = fileInfo.fileName();
  dataSetTableName = manager.getTableName(dataSetName);
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::OverwriteExisting));
  // Get model and check that columns exists
  model = manager.model();
  QVERIFY(model != 0);
  QCOMPARE(model->columnCount(), 3);
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("value"));

  /*
   * Tests in OnRowChange edit strategy
   */
  model->setEditStrategy(QSqlTableModel::OnRowChange);
  // Check that data set is empty
  QVERIFY(model->select());
  QCOMPARE(model->rowCount(), 0);
  // Add a row with valid data - method A
  rowData.insert("signal", "Cmp. temp");
  rowData.insert("value", 25.4);
  QVERIFY(model->addRow(rowData));
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  // Add a row with to much fields - method A
  rowData.insert("fake", "??");
  QVERIFY(!model->addRow(rowData));
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  // Add a row with "to" less fields (must work) - method A
  rowData.clear();
  rowData.insert("signal", "Temp. M1");
  QVERIFY(model->addRow(rowData));
  QCOMPARE(model->rowCount(), 2);
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  // Add a row with wrong fields - method A
  rowData.clear();
  rowData.insert("wrongField", "????");
  QVERIFY(!model->addRow(rowData));
  QCOMPARE(model->rowCount(), 2);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  // Add a row with valid data - method B
  QVERIFY(model->insertRows(0, 1));
  QVERIFY(model->setData(0, 1, "V soll"));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Add a row with invalid row - method B
  QVERIFY(model->insertRows(0, 1));
  QVERIFY(!model->setData(15, 1, "Fake..."));
  model->revertRow(0);
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Add a row with invalid column - method B
  QVERIFY(model->insertRows(0, 1));
  QVERIFY(!model->setData(0, 15, "Fake..."));
  model->revertRow(0);
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Edit a row with valid data - method A
  QVERIFY(model->setData(2, 2, 56.4));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with invalid row - method A
  QVERIFY(!model->setData(20, 2, 44.2));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with invalid column - method A
  QVERIFY(!model->setData(2, 21, 42.5));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with valid data - method B
  QVERIFY(model->setData(2, "value", 88.4));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Edit a row with invalid row - method B
  QVERIFY(!model->setData(25, "value", 78));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Edit a row with invalid column - method B
  QVERIFY(!model->setData(2, "fake..", 68.2));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Add a row with valid data - method A2
  rowData2.clear();
  rowData2 << "V ist" << 92.1;
  QVERIFY(model->addRow(rowData2, true));
  QCOMPARE(model->rowCount(), 4);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  QCOMPARE(model->data(model->index(3, 1)), QVariant("V ist"));
  QCOMPARE(model->data(model->index(3, 2)), QVariant(92.1));
  // Add a row with valid data - method A3
  rowData3.clear();
  rowData3 << "Temp. M2" << "125.4";
  QVERIFY(model->addRow(rowData3, true));
  QCOMPARE(model->rowCount(), 5);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  QCOMPARE(model->data(model->index(3, 1)), QVariant("V ist"));
  QCOMPARE(model->data(model->index(3, 2)), QVariant(92.1));
  QCOMPARE(model->data(model->index(4, 1)), QVariant("Temp. M2"));
  QCOMPARE(model->data(model->index(4, 2)), QVariant(125.4));

  /*
   * Tests in OnManualSubmit edit strategy
   */
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  // Check that data set is empty
  QVERIFY(model->removeRows(0, model->rowCount()));
  QVERIFY(model->submitAll());
  QCOMPARE(model->rowCount(), 0);
  QCOMPARE(model->data(model->index(0, 1)), QVariant());
  // Add a row with valid data - method A
  rowData.clear();
  rowData.insert("signal", "Cmp. temp");
  rowData.insert("value", 25.4);
  QVERIFY(model->addRow(rowData));
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  // Add a row with to much fields - method A
  rowData.insert("fake", "??");
  QVERIFY(!model->addRow(rowData));
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  // Add a row with "to" less fields (must work) - method A
  rowData.clear();
  rowData.insert("signal", "Temp. M1");
  QVERIFY(model->addRow(rowData));
  QCOMPARE(model->rowCount(), 2);
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  // Add a row with wrong fields - method A
  rowData.clear();
  rowData.insert("wrongField", "????");
  QVERIFY(!model->addRow(rowData));
  QCOMPARE(model->rowCount(), 2);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  // Add a row with valid data - method B
  QVERIFY(model->insertRows(model->rowCount(), 1));
  QVERIFY(model->setData(model->rowCount()-1, 1, "V soll"));
  QVERIFY(model->submitAll());
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  model->revertAll();
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Add a row with invalid row - method B
  QVERIFY(model->insertRows(0, 1));
  QVERIFY(!model->setData(15, 1, "Fake..."));
  model->revertRow(0);
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Add a row with invalid column - method B
  QVERIFY(model->insertRows(0, 1));
  QVERIFY(!model->setData(0, 15, "Fake..."));
  model->revertRow(0);
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  // Edit a row with valid data - method A
  QVERIFY(model->setData(2, 2, 56.4));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with invalid row - method A
  QVERIFY(!model->setData(20, 2, 44.2));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with invalid column - method A
  QVERIFY(!model->setData(2, 21, 42.5));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(56.4));
  // Edit a row with valid data - method B
  QVERIFY(model->setData(2, "value", 88.4));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Edit a row with invalid row - method B
  QVERIFY(!model->setData(25, "value", 78));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Edit a row with invalid column - method B
  QVERIFY(!model->setData(2, "fake..", 68.2));
  QCOMPARE(model->rowCount(), 3);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  // Add a row with valid data - method A2
  rowData2.clear();
  rowData2 << "V ist" << 92.1;
  QVERIFY(model->addRow(rowData2, true));
  QCOMPARE(model->rowCount(), 4);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  QCOMPARE(model->data(model->index(3, 1)), QVariant("V ist"));
  QCOMPARE(model->data(model->index(3, 2)), QVariant(92.1));
  // Add a row with valid data - method A3
  rowData3.clear();
  rowData3 << "Temp. M2" << "125.4";
  QVERIFY(model->addRow(rowData3, true));
  QCOMPARE(model->rowCount(), 5);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  QCOMPARE(model->data(model->index(3, 1)), QVariant("V ist"));
  QCOMPARE(model->data(model->index(3, 2)), QVariant(92.1));
  QCOMPARE(model->data(model->index(4, 1)), QVariant("Temp. M2"));
  QCOMPARE(model->data(model->index(4, 2)), QVariant(125.4));
  // Add a rows with valid data - method A4 - PK is not given in rows
  fieldMap.clear();
  mapItem = new mdtFieldMapItem;
  mapItem->setDestinationFieldIndex(0);
  mapItem->setDestinationFieldName("id_PK");
  mapItem->setSourceFieldIndex(-1);
  mapItem->setSourceFieldName("id_PK");
  mapItem->setDataType(QVariant::Int);
  fieldMap.addItem(mapItem);
  mapItem = new mdtFieldMapItem;
  mapItem->setDestinationFieldIndex(1);
  mapItem->setDestinationFieldName("signal");
  mapItem->setSourceFieldIndex(0);
  mapItem->setSourceFieldName("signal");
  mapItem->setDataType(QVariant::String);
  fieldMap.addItem(mapItem);
  mapItem = new mdtFieldMapItem;
  mapItem->setDestinationFieldIndex(2);
  mapItem->setDestinationFieldName("value");
  mapItem->setSourceFieldIndex(1);
  mapItem->setSourceFieldName("value");
  mapItem->setDataType(QVariant::Double);
  fieldMap.addItem(mapItem);
  rowsData4.clear();
  rowData3.clear();
  rowData3 << "Temp. M3" << "123.2";
  rowsData4 << rowData3;
  rowData3.clear();
  rowData3 << "Temp. M4" << "89.7";
  rowsData4 << rowData3;
  QVERIFY(model->addRows(rowsData4, fieldMap));
  QCOMPARE(model->rowCount(), 7);
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant(25.4));
  QCOMPARE(model->data(model->index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(2, 1)), QVariant("V soll"));
  QCOMPARE(model->data(model->index(2, 2)), QVariant(88.4));
  QCOMPARE(model->data(model->index(3, 1)), QVariant("V ist"));
  QCOMPARE(model->data(model->index(3, 2)), QVariant(92.1));
  QCOMPARE(model->data(model->index(4, 1)), QVariant("Temp. M2"));
  QCOMPARE(model->data(model->index(4, 2)), QVariant(125.4));
  QCOMPARE(model->data(model->index(5, 1)), QVariant("Temp. M3"));
  QCOMPARE(model->data(model->index(5, 2)), QVariant(123.2));
  QCOMPARE(model->data(model->index(6, 1)), QVariant("Temp. M4"));
  QCOMPARE(model->data(model->index(6, 2)), QVariant(89.7));
}

void mdtDataTableTest::fieldMapItemCopyTest()
{
  mdtFieldMapItem item1, item2;
  QList<mdtFieldMapField> sourceFields, destinationFields;

  // Setup item1 and check
  item1.setDestinationFieldIndex(1);
  item1.setDestinationFieldName("DEST1");
  item1.setDestinationFieldDisplayText("Destination 1");
  item1.setSourceFieldIndex(11);
  item1.setSourceFieldName("SRC1");
  item1.setSourceFieldDataStartOffset(10);
  item1.setSourceFieldDataEndOffset(15);
  item1.setDataType(QVariant::Int);
  QCOMPARE(item1.destinationFieldIndex(), 1);
  QCOMPARE(item1.destinationFieldName(), QString("DEST1"));
  QCOMPARE(item1.destinationFieldDisplayText(), QString("Destination 1"));
  QCOMPARE(item1.sourceFieldIndex(), 11);
  QCOMPARE(item1.sourceFieldName(), QString("SRC1"));
  QCOMPARE(item1.sourceFieldDataStartOffset(), 10);
  QCOMPARE(item1.sourceFieldDataEndOffset(), 15);
  QCOMPARE(item1.dataType(), QVariant::Int);
  // Setup item2 and check
  item2.setDestinationFieldIndex(2);
  item2.setDestinationFieldName("DEST2");
  item2.setDestinationFieldDisplayText("Destination 2");
  item2.setSourceFieldIndex(12);
  item2.setSourceFieldName("SRC2");
  item2.setSourceFieldDataStartOffset(20);
  item2.setSourceFieldDataEndOffset(25);
  item2.setDataType(QVariant::Int);
  QCOMPARE(item2.destinationFieldIndex(), 2);
  QCOMPARE(item2.destinationFieldName(), QString("DEST2"));
  QCOMPARE(item2.destinationFieldDisplayText(), QString("Destination 2"));
  QCOMPARE(item2.sourceFieldIndex(), 12);
  QCOMPARE(item2.sourceFieldName(), QString("SRC2"));
  QCOMPARE(item2.sourceFieldDataStartOffset(), 20);
  QCOMPARE(item2.sourceFieldDataEndOffset(), 25);
  QCOMPARE(item2.dataType(), QVariant::Int);
  // Copy item1 -> item2 and check item2
  item2 = item1;
  QCOMPARE(item2.destinationFieldIndex(), 1);
  QCOMPARE(item2.destinationFieldName(), QString("DEST1"));
  QCOMPARE(item2.destinationFieldDisplayText(), QString("Destination 1"));
  QCOMPARE(item2.sourceFieldIndex(), 11);
  QCOMPARE(item2.sourceFieldName(), QString("SRC1"));
  QCOMPARE(item2.sourceFieldDataStartOffset(), 10);
  QCOMPARE(item2.sourceFieldDataEndOffset(), 15);
  QCOMPARE(item2.dataType(), QVariant::Int);
  // Modify item1 and check that item2 contains they own data
  item1.setDestinationFieldIndex(3);
  item1.setDestinationFieldName("DEST3");
  item1.setDestinationFieldDisplayText("Destination 3");
  item1.setSourceFieldIndex(13);
  item1.setSourceFieldName("SRC3");
  item1.setSourceFieldDataStartOffset(30);
  item1.setSourceFieldDataEndOffset(35);
  item1.setDataType(QVariant::Int);
  QCOMPARE(item1.destinationFieldIndex(), 3);
  QCOMPARE(item1.destinationFieldName(), QString("DEST3"));
  QCOMPARE(item1.destinationFieldDisplayText(), QString("Destination 3"));
  QCOMPARE(item1.sourceFieldIndex(), 13);
  QCOMPARE(item1.sourceFieldName(), QString("SRC3"));
  QCOMPARE(item1.sourceFieldDataStartOffset(), 30);
  QCOMPARE(item1.sourceFieldDataEndOffset(), 35);
  QCOMPARE(item1.dataType(), QVariant::Int);
  QCOMPARE(item2.destinationFieldIndex(), 1);
  QCOMPARE(item2.destinationFieldName(), QString("DEST1"));
  QCOMPARE(item2.destinationFieldDisplayText(), QString("Destination 1"));
  QCOMPARE(item2.sourceFieldIndex(), 11);
  QCOMPARE(item2.sourceFieldName(), QString("SRC1"));
  QCOMPARE(item2.sourceFieldDataStartOffset(), 10);
  QCOMPARE(item2.sourceFieldDataEndOffset(), 15);
  QCOMPARE(item2.dataType(), QVariant::Int);
}

void mdtDataTableTest::fieldMapCopyTest()
{
  mdtFieldMap map1, map2;
  mdtFieldMapItem *item;
  QList<mdtFieldMapField> sourceFields, destinationFields;
  mdtFieldMapField field;

  /*
   * Simple test with most of data members
   */

  // Setup map1 and check
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(11);
  item->setDestinationFieldName("DEST11");
  item->setDestinationFieldDisplayText("Destination 11");
  item->setSourceFieldIndex(111);
  item->setSourceFieldName("SRC11");
  item->setDataType(QVariant::String);
  map1.addItem(item);
  item = 0;
  item = map1.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 11);
  QCOMPARE(item->destinationFieldName(), QString("DEST11"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Destination 11"));
  QCOMPARE(item->sourceFieldIndex(), 111);
  QCOMPARE(item->sourceFieldName(), QString("SRC11"));
  QCOMPARE(item->dataType(), QVariant::String);
  // Setup map2 and check
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(21);
  item->setDestinationFieldName("DEST21");
  item->setDestinationFieldDisplayText("Destination 21");
  item->setSourceFieldIndex(211);
  item->setSourceFieldName("SRC21");
  item->setDataType(QVariant::String);
  map2.addItem(item);
  item = 0;
  item = map2.itemAtFieldIndex(21);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 21);
  QCOMPARE(item->destinationFieldName(), QString("DEST21"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Destination 21"));
  QCOMPARE(item->sourceFieldIndex(), 211);
  QCOMPARE(item->sourceFieldName(), QString("SRC21"));
  QCOMPARE(item->dataType(), QVariant::String);
  // Copy map1 to map2 and check
  map2 = map1;
  item = 0;
  item = map2.itemAtFieldIndex(21);
  QVERIFY(item == 0);
  item = map2.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 11);
  QCOMPARE(item->destinationFieldName(), QString("DEST11"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Destination 11"));
  QCOMPARE(item->sourceFieldIndex(), 111);
  QCOMPARE(item->sourceFieldName(), QString("SRC11"));
  QCOMPARE(item->dataType(), QVariant::String);
  // Modify map1 and check that map2 is keeped untouched
  map1.clear();
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(13);
  item->setDestinationFieldName("DEST13");
  item->setDestinationFieldDisplayText("Destination 13");
  item->setSourceFieldIndex(113);
  item->setSourceFieldName("SRC13");
  item->setDataType(QVariant::String);
  map1.addItem(item);
  // Check back map1 (must have new data)
  item = map1.itemAtFieldIndex(11);
  QVERIFY(item == 0);
  item = map1.itemAtFieldIndex(13);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 13);
  QCOMPARE(item->destinationFieldName(), QString("DEST13"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Destination 13"));
  QCOMPARE(item->sourceFieldIndex(), 113);
  QCOMPARE(item->sourceFieldName(), QString("SRC13"));
  QCOMPARE(item->dataType(), QVariant::String);
  // Check that map2 is intact
  item = map2.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 11);
  QCOMPARE(item->destinationFieldName(), QString("DEST11"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Destination 11"));
  QCOMPARE(item->sourceFieldIndex(), 111);
  QCOMPARE(item->sourceFieldName(), QString("SRC11"));
  QCOMPARE(item->dataType(), QVariant::String);

  /*
   * Make a little bit more tests, but only with few data members
   */

  // Fill source fields
  sourceFields.clear();
  field.setName("SRC01");
  sourceFields << field;
  field.setName("SRC02");
  sourceFields << field;
  field.setName("SRC03");
  sourceFields << field;
  field.setName("SRC04");
  sourceFields << field;
  // Fill destination fields
  destinationFields.clear();
  field.setName("DEST01");
  destinationFields << field;
  field.setName("DEST02");
  destinationFields << field;
  field.setName("DEST03");
  destinationFields << field;

  // Begin with at 0
  map1.clear();
  map2.clear();
  QCOMPARE(map1.items().size(), 0);
  QCOMPARE(map2.items().size(), 0);
  // Put data into map1
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(10);
  map1.addItem(item);
  QCOMPARE(map1.items().size(), 1);
  item = map1.itemAtFieldIndex(10);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 10);
  QCOMPARE(map1.sourceFields().size(), 0);
  QCOMPARE(map1.destinationFields().size(), 0);
  // Put data into map2
  map2.setSourceFields(sourceFields);
  map2.setDestinationFields(destinationFields);
  QCOMPARE(map2.sourceFields().size(), sourceFields.size());
  QCOMPARE(map2.destinationFields().size(), destinationFields.size());
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(20);
  map2.addItem(item);
  QCOMPARE(map2.items().size(), 1);
  item = map2.itemAtFieldIndex(20);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 20);
  // Copy map2 to map1 and check
  map1 = map2;
  QCOMPARE(map1.items().size(), 1);
  QCOMPARE(map2.items().size(), 1);
  item = map1.itemAtFieldIndex(20);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 20);
  QCOMPARE(map1.sourceFields().size(), sourceFields.size());
  QCOMPARE(map1.destinationFields().size(), destinationFields.size());
  item = map2.itemAtFieldIndex(20);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 20);
  QCOMPARE(map2.sourceFields().size(), sourceFields.size());
  QCOMPARE(map2.destinationFields().size(), destinationFields.size());
  // Modify map1
  item = new mdtFieldMapItem;
  item->setDestinationFieldIndex(11);
  map1.addItem(item);
  // Check map1
  QCOMPARE(map1.items().size(), 2);
  item = map1.itemAtFieldIndex(20);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 20);
  item = map1.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 11);
  QCOMPARE(map1.sourceFields().size(), sourceFields.size());
  QCOMPARE(map1.destinationFields().size(), destinationFields.size());
  // Check map2 - must be untouched
  QCOMPARE(map2.items().size(), 1);
  item = map2.itemAtFieldIndex(20);
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 20);
  QCOMPARE(map2.sourceFields().size(), sourceFields.size());
  QCOMPARE(map2.destinationFields().size(), destinationFields.size());
}

void mdtDataTableTest::fieldMapFieldsMappedFilterTest()
{
  mdtFieldMap map;
  mdtFieldMapItem *item;
  QList<mdtFieldMapField> sourceFields, destinationFields;
  mdtFieldMapField field;

  // Fill source fields
  sourceFields.clear();
  field.setName("SRC01");
  sourceFields << field;
  field.setName("SRC02");
  sourceFields << field;
  field.setName("SRC03");
  sourceFields << field;
  field.setName("SRC04");
  sourceFields << field;
  // Fill destination fields
  destinationFields.clear();
  field.setName("DEST01");
  destinationFields << field;
  field.setName("DEST02");
  destinationFields << field;
  field.setName("DEST03");
  destinationFields << field;

  // Initial check - source fields
  map.setSourceFields(sourceFields);
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).size(), 4);
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(0).name(), QString("SRC01"));
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(1).name(), QString("SRC02"));
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(2).name(), QString("SRC03"));
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(3).name(), QString("SRC04"));
  // Initial check - destination fields
  map.setDestinationFields(destinationFields);
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).size(), 3);
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).at(0).name(), QString("DEST01"));
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).at(1).name(), QString("DEST02"));
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).at(2).name(), QString("DEST03"));

  // Create a map and check
  item = new mdtFieldMapItem;
  item->setSourceFieldName("SRC01");
  item->setDestinationFieldName("DEST01");
  map.addItem(item);
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).size(), 3);
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(0).name(), QString("SRC02"));
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(1).name(), QString("SRC03"));
  QCOMPARE(map.notMappedSourceFields(mdtFieldMap::ReferenceByName).at(2).name(), QString("SRC04"));
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).size(), 2);
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).at(0).name(), QString("DEST02"));
  QCOMPARE(map.notMappedDestinationFields(mdtFieldMap::ReferenceByName).at(1).name(), QString("DEST03"));

}

void mdtDataTableTest::fieldMapHeaderTest()
{
  mdtFieldMap map;
  mdtFieldMapItem *item;
  QStringList expectedSourceHeader, expectedDestinationHeader;

  // Setup expected headers
  expectedSourceHeader << "Source 1" << "Source 2" << "SRC3" << "Source 4" << "Source 5";
  expectedDestinationHeader << "Destination 1" << "Destination 2" << "DEST3" << "Destination 4" << "Destination 5";

  // Build the map
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(0);
  item->setSourceFieldName("SRC1");
  item->setSourceFieldDisplayText("Source 1");
  item->setDestinationFieldIndex(0);
  item->setDestinationFieldName("DEST1");
  item->setDestinationFieldDisplayText("Destination 1");
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(2);
  item->setSourceFieldName("SRC3");
  item->setSourceFieldDisplayText("");
  item->setDestinationFieldIndex(2);
  item->setDestinationFieldName("DEST3");
  item->setDestinationFieldDisplayText("");
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(1);
  item->setSourceFieldName("SRC2");
  item->setSourceFieldDisplayText("Source 2");
  item->setDestinationFieldIndex(1);
  item->setDestinationFieldName("DEST2");
  item->setDestinationFieldDisplayText("Destination 2");
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("SRC5");
  item->setSourceFieldDisplayText("Source 5");
  item->setDestinationFieldIndex(3);
  item->setDestinationFieldName("DEST4");
  item->setDestinationFieldDisplayText("Destination 4");
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(3);
  item->setSourceFieldName("SRC4");
  item->setSourceFieldDisplayText("Source 4");
  item->setDestinationFieldIndex(4);
  item->setDestinationFieldName("DEST5");
  item->setDestinationFieldDisplayText("Destination 5");
  map.addItem(item);
  // Check
  QCOMPARE(map.sourceHeader(), expectedSourceHeader);
  QCOMPARE(map.destinationHeader(), expectedDestinationHeader);
}

void mdtDataTableTest::fieldMapTest()
{
  mdtFieldMap map;
  mdtFieldMapItem *item;
  QList<mdtFieldMapItem*> items;
  QStringList csvHeader;
  QStringList csvLine;
  QStringList modelHeader;
  QList<QVariant> modelRow;
  QHash<QString, QString> displayTextsByFieldNames;

  // Build source fields and data
  csvHeader << "A" << "B" << "Group1" << "C" << "Group2";
  csvLine << "A data" << "B data" << "12Name 12" << "C data" << "123Na123Value123";
  // Build model fields and data
  modelHeader << "A" << "B" << "Sub1Id" << "Sub1Name" << "C" << "Sub2Id" << "Sub2Name" << "Sub2Value";
  modelRow << "A data" << "B data" << "12" << "Name 12" << "C data" << "123" << "Na123" << "Value123";
  // Build the map
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(0);
  item->setSourceFieldName("A");
  item->setDestinationFieldIndex(0);
  item->setDestinationFieldName("A");
  item->setDestinationFieldDisplayText("A");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(1);
  item->setSourceFieldName("B");
  item->setDestinationFieldIndex(1);
  item->setDestinationFieldName("B");
  item->setDestinationFieldDisplayText("B");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(2);
  item->setSourceFieldName("Group1");
  item->setSourceFieldDataStartOffset(0);
  item->setSourceFieldDataEndOffset(1);
  item->setDestinationFieldIndex(2);
  item->setDestinationFieldName("Sub1Id");
  item->setDestinationFieldDisplayText("Grp 1 ID");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(2);
  item->setSourceFieldName("Group1");
  item->setSourceFieldDataStartOffset(2);
  item->setSourceFieldDataEndOffset(-1);
  item->setDestinationFieldIndex(3);
  item->setDestinationFieldName("Sub1Name");
  item->setDestinationFieldDisplayText("Grp 1 Name");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(3);
  item->setSourceFieldName("C");
  item->setDestinationFieldIndex(4);
  item->setDestinationFieldName("C");
  item->setDestinationFieldDisplayText("C");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(0);
  item->setSourceFieldDataEndOffset(2);
  item->setDestinationFieldIndex(5);
  item->setDestinationFieldName("Sub2Id");
  item->setDestinationFieldDisplayText("Grp 2 ID");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(3);
  item->setSourceFieldDataEndOffset(7);
  item->setDestinationFieldIndex(6);
  item->setDestinationFieldName("Sub2Name");
  item->setDestinationFieldDisplayText("Grp 2 Name");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(8);
  item->setSourceFieldDataEndOffset(15);
  item->setDestinationFieldIndex(7);
  item->setDestinationFieldName("Sub2Value");
  item->setDestinationFieldDisplayText("Grp 2 Value");
  item->setDataType(QVariant::String);
  map.addItem(item);
  // Check that fields are correctly mapped
  for(int i=0; i<csvHeader.size(); i++){
    items = map.itemsAtSourceFieldIndex(i);
    for(int j=0; j<items.size(); j++){
      item = items.at(j);
      QVERIFY(item != 0);
      QCOMPARE(item->sourceFieldIndex(), i);
      QCOMPARE(item->sourceFieldName(), csvHeader.at(i));
    }
    items = map.itemsAtSourceFieldName(csvHeader.at(i));
    for(int j=0; j<items.size(); j++){
      item = items.at(j);
      QVERIFY(item != 0);
      QCOMPARE(item->sourceFieldIndex(), i);
      QCOMPARE(item->sourceFieldName(), csvHeader.at(i));
    }
  }
  for(int i=0; i<modelHeader.size(); i++){
    item = map.itemAtFieldIndex(i);
    QVERIFY(item != 0);
    QCOMPARE(item->destinationFieldIndex(), i);
    QCOMPARE(item->destinationFieldName(), modelHeader.at(i));
    item = map.itemAtFieldName(modelHeader.at(i));
    QVERIFY(item != 0);
    QCOMPARE(item->destinationFieldIndex(), i);
    QCOMPARE(item->destinationFieldName(), modelHeader.at(i));
  }
  // Check map direct access
  QCOMPARE(map.sourceFieldNameAtFieldIndex(0), QString("A"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(1), QString("B"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(2), QString("Group1"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(3), QString("Group1"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(4), QString("C"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(5), QString("Group2"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(6), QString("Group2"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(7), QString("Group2"));
  QCOMPARE(map.sourceFieldNameAtFieldIndex(8), QString(""));
  // Check Group1 split
  item = map.itemAtFieldName("Sub1Id");
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 2);
  QCOMPARE(item->destinationFieldName(), QString("Sub1Id"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Grp 1 ID"));
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("Group1"));
  QCOMPARE(item->sourceFieldDataStartOffset(), 0);
  QCOMPARE(item->sourceFieldDataEndOffset(), 1);
  item = map.itemAtFieldName("Sub1Name");
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 3);
  QCOMPARE(item->destinationFieldName(), QString("Sub1Name"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Grp 1 Name"));
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("Group1"));
  // Check Group2 split
  item = map.itemAtFieldName("Sub2Id");
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 5);
  QCOMPARE(item->destinationFieldName(), QString("Sub2Id"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Grp 2 ID"));
  QCOMPARE(item->sourceFieldIndex(), 4);
  QCOMPARE(item->sourceFieldName(), QString("Group2"));
  item = map.itemAtFieldName("Sub2Name");
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 6);
  QCOMPARE(item->destinationFieldName(), QString("Sub2Name"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Grp 2 Name"));
  QCOMPARE(item->sourceFieldIndex(), 4);
  QCOMPARE(item->sourceFieldName(), QString("Group2"));
  item = map.itemAtFieldName("Sub2Value");
  QVERIFY(item != 0);
  QCOMPARE(item->destinationFieldIndex(), 7);
  QCOMPARE(item->destinationFieldName(), QString("Sub2Value"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("Grp 2 Value"));
  QCOMPARE(item->sourceFieldIndex(), 4);
  QCOMPARE(item->sourceFieldName(), QString("Group2"));
  // Check data map
  QCOMPARE(modelHeader.size(), modelRow.size());
  for(int i=0; i<modelRow.size(); i++){
    QCOMPARE(map.dataForFieldIndex(csvLine, i), QVariant(modelRow.at(i)));
    QCOMPARE(map.dataForFieldName(csvLine, modelHeader.at(i)), QVariant(modelRow.at(i)));
  }
  QCOMPARE(map.dataForDisplayText(csvLine, "Grp 2 Name"), QVariant("Na123"));
  QCOMPARE(csvHeader.size(), csvLine.size());
  for(int i=0; i<csvLine.size(); i++){
    QCOMPARE(map.dataForSourceFieldIndex(modelRow, i), csvLine.at(i));
    QCOMPARE(map.dataForSourceFieldName(modelRow, csvHeader.at(i)), csvLine.at(i));
  }
  /*
   * Check that index update works
   */
  map.clear();
  // Build the map without specifing indexes
  item = new mdtFieldMapItem;
  item->setSourceFieldName("A");
  item->setDestinationFieldName("a1");
  item->setDestinationFieldDisplayText("A 1");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("A");
  item->setDestinationFieldName("a2");
  item->setDestinationFieldDisplayText("A 2");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("B");
  item->setDestinationFieldName("b");
  item->setDestinationFieldDisplayText("B");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("C");
  item->setDestinationFieldName("c");
  item->setDestinationFieldDisplayText("C");
  item->setDataType(QVariant::String);
  map.addItem(item);
  // Check the mapping
  item = map.itemAtFieldName("a1");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->destinationFieldName(), QString("a1"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("A 1"));
  item = map.itemAtFieldName("a2");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->destinationFieldName(), QString("a2"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("A 2"));
  item = map.itemAtFieldName("b");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->destinationFieldName(), QString("b"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("B"));
  item = map.itemAtFieldName("c");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->destinationFieldName(), QString("c"));
  QCOMPARE(item->destinationFieldDisplayText(), QString("C"));
  // Update indexes
  item = map.itemAtFieldName("a1");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(1);
  item->setDestinationFieldIndex(10);
  item = map.itemAtFieldName("a2");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(1);
  item->setDestinationFieldIndex(11);
  item = map.itemAtFieldName("b");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(2);
  item->setDestinationFieldIndex(12);
  item = map.itemAtFieldName("c");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(3);
  item->setDestinationFieldIndex(13);
  // Check the mapping - search by field index
  item = map.itemAtFieldIndex(10);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->destinationFieldIndex(), 10);
  QCOMPARE(item->destinationFieldName(), QString("a1"));
  item = map.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->destinationFieldIndex(), 11);
  QCOMPARE(item->destinationFieldName(), QString("a2"));
  item = map.itemAtFieldIndex(12);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->destinationFieldIndex(), 12);
  QCOMPARE(item->destinationFieldName(), QString("b"));
  item = map.itemAtFieldIndex(13);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 3);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->destinationFieldIndex(), 13);
  QCOMPARE(item->destinationFieldName(), QString("c"));
  // Check the mapping - search by source field index
  items = map.itemsAtSourceFieldIndex(1);
  QVERIFY(items.size() == 2);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  // We don't know how items are ordered by field index
  if(item->destinationFieldIndex() == 10){
    QCOMPARE(item->destinationFieldName(), QString("a1"));
    item = items.at(1);
    QVERIFY(item != 0);
    QCOMPARE(item->sourceFieldIndex(), 1);
    QCOMPARE(item->sourceFieldName(), QString("A"));
    QCOMPARE(item->destinationFieldIndex(), 11);
    QCOMPARE(item->destinationFieldName(), QString("a2"));
  }else if(item->destinationFieldIndex() == 11){
    QCOMPARE(item->destinationFieldName(), QString("a2"));
    item = items.at(1);
    QVERIFY(item != 0);
    QCOMPARE(item->sourceFieldIndex(), 1);
    QCOMPARE(item->sourceFieldName(), QString("A"));
    QCOMPARE(item->destinationFieldIndex(), 10);
    QCOMPARE(item->destinationFieldName(), QString("a1"));

  }
  items = map.itemsAtSourceFieldIndex(2);
  QVERIFY(items.size() == 1);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->destinationFieldIndex(), 12);
  QCOMPARE(item->destinationFieldName(), QString("b"));
  items = map.itemsAtSourceFieldIndex(3);
  QVERIFY(items.size() == 1);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 3);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->destinationFieldIndex(), 13);
  QCOMPARE(item->destinationFieldName(), QString("c"));
  // Check display texts by field names
  displayTextsByFieldNames = map.displayTextsByFieldNames();
  QCOMPARE(displayTextsByFieldNames.size(), 4);
  QCOMPARE(displayTextsByFieldNames.value("a1"), QString("A 1"));
  QCOMPARE(displayTextsByFieldNames.value("a2"), QString("A 2"));
  QCOMPARE(displayTextsByFieldNames.value("b"), QString("B"));
  QCOMPARE(displayTextsByFieldNames.value("c"), QString("C"));
}

void mdtDataTableTest::csvExportTest()
{
  mdtDataTableManager manager;
  QTemporaryFile dbFile;
  QFileInfo fileInfo;
  QSqlField field;
  QString dataSetName;
  QString dataSetTableName;
  QMap<QString,QVariant> rowData;
  mdtSqlSchemaTable table;

  // Build fields
  field = QSqlField();
  field.setName("id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  table.addField(field, true);
  field = QSqlField();
  field.setName("signal");
  field.setType(QVariant::String);
  table.addField(field, false);
  field = QSqlField();
  field.setName("value");
  field.setType(QVariant::Double);
  table.addField(field, false);

  // Create data set
  QVERIFY(dbFile.open());
  fileInfo.setFile(dbFile);
  dataSetName = fileInfo.fileName();
  dataSetTableName = manager.getTableName(dataSetName);
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtSqlDatabaseManager::OverwriteExisting));
  // Set model and check that columns exists
  mdtDataTableModel m(0, manager.database());
  m.setTable(table.tableName());
  QVERIFY(m.select());
  QCOMPARE(m.columnCount(), 3);
  QCOMPARE(m.headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(m.headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(m.headerData(2, Qt::Horizontal), QVariant("value"));

  QSKIP("Test not completly implemented", SkipAll);
}

void mdtDataTableTest::csvImportTest()
{
  QTemporaryFile csvFile1, csvFile2, csvFile3;
  mdtDataTableManager manager;
  mdtDataTableModel *model;
  QStringList csvHeader;

  // Write CSV file that contains a primary key
  QVERIFY(csvFile1.open());
  QVERIFY(csvFile1.write("code_PK;signal;value" + QByteArray(MDT_NATIVE_EOL)) > 0);
  QVERIFY(csvFile1.write("1;Temp. M1;125.0" + QByteArray(MDT_NATIVE_EOL)) > 0);
  csvFile1.close();
  // Configure manager and import CSV
  manager.setCsvFormat(";", "", "", '\0');
  QVERIFY(manager.importFromCsvFile(csvFile1.fileName(), mdtSqlDatabaseManager::OverwriteExisting, "", QStringList("code_PK")));
  model = manager.model();
  QVERIFY(model != 0);
  // Check model's header
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("code_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("value"));
  // Check CSV headers
  csvHeader = manager.csvHeader();
  QCOMPARE(csvHeader.size(), 3);
  QCOMPARE(csvHeader.at(0), QString("code_PK"));
  QCOMPARE(csvHeader.at(1), QString("signal"));
  QCOMPARE(csvHeader.at(2), QString("value"));
  // Check imported data
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 0)), QVariant(1));
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Temp. M1"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant("125.0"));

  // Write CSV file that contains no primary key
  QVERIFY(csvFile2.open());
  QVERIFY(csvFile2.write("signal;value" + QByteArray(MDT_NATIVE_EOL)) > 0);
  QVERIFY(csvFile2.write("Temp. M2;126.0" + QByteArray(MDT_NATIVE_EOL)) > 0);
  csvFile2.close();
  // Configure manager and import CSV
  manager.clearFieldMap();
  manager.setCsvFormat(";", "", "", '\0');
  QVERIFY(manager.importFromCsvFile(csvFile2.fileName(), mdtSqlDatabaseManager::OverwriteExisting));
  model = manager.model();
  QVERIFY(model != 0);
  // Check model's header
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("value"));
  // Check CSV headers
  csvHeader = manager.csvHeader();
  QCOMPARE(csvHeader.size(), 2);
  QCOMPARE(csvHeader.at(0), QString("signal"));
  QCOMPARE(csvHeader.at(1), QString("value"));
  // Check imported data
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 0)), QVariant(1));
  QCOMPARE(model->data(model->index(0, 1)), QVariant("Temp. M2"));
  QCOMPARE(model->data(model->index(0, 2)), QVariant("126.0"));

  /*
   * Check field mapping
   */

  // Write CSV file that contains no primary key
  QVERIFY(csvFile3.open());
  QVERIFY(csvFile3.write("bin I/O;signal;value" + QByteArray(MDT_NATIVE_EOL)) > 0);
  QVERIFY(csvFile3.write("10;Rpm M2;55.0" + QByteArray(MDT_NATIVE_EOL)) > 0);
  csvFile3.close();
  // Configure manager and import CSV
  manager.clearFieldMap();
  manager.setCsvFormat(";", "", "", '\0');
  manager.addFieldMapping("bin I/O", "powerState", "Power state", QVariant::String, 0, 0);
  manager.addFieldMapping("bin I/O", "cpuState", "CPU state", QVariant::String, 1, 1);
  manager.addFieldMapping("value", "value", "Value", QVariant::Double);
  QVERIFY(manager.importFromCsvFile(csvFile3.fileName(), mdtSqlDatabaseManager::OverwriteExisting));
  model = manager.model();
  QVERIFY(model != 0);
  // Check model's header. Note: we don't know in witch order field splitting is done
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  if(model->headerData(1, Qt::Horizontal) == QVariant("powerState")){
    QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("cpuState"));
  }else{
    QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("cpuState"));
    QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("powerState"));
  }
  QCOMPARE(model->headerData(3, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(4, Qt::Horizontal), QVariant("value"));
  // Check CSV headers
  csvHeader = manager.csvHeader();
  QCOMPARE(csvHeader.size(), 3);
  QCOMPARE(csvHeader.at(0), QString("bin I/O"));
  QCOMPARE(csvHeader.at(1), QString("signal"));
  QCOMPARE(csvHeader.at(2), QString("value"));
  // Check imported data
  QCOMPARE(model->rowCount(), 1);
  QCOMPARE(model->data(model->index(0, 0)), QVariant(1));
  if(model->headerData(1, Qt::Horizontal) == QVariant("powerState")){
    QCOMPARE(model->data(model->index(0, 1)), QVariant("1"));
    QCOMPARE(model->data(model->index(0, 2)), QVariant("0"));
  }else{
    QCOMPARE(model->data(model->index(0, 1)), QVariant("0"));
    QCOMPARE(model->data(model->index(0, 2)), QVariant("1"));
  }
  QCOMPARE(model->data(model->index(0, 3)), QVariant("Rpm M2"));
  QCOMPARE(model->data(model->index(0, 4)), QVariant(55.0));
  // Set display texts to model header
  manager.setDisplayTextsToModelHeader();
  // Check model's header. Note: we don't know in witch order field splitting is done
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  if(model->headerData(1, Qt::Horizontal) == QVariant("Power state")){
    QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("Power state"));
    QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("CPU state"));
  }else{
    QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("CPU state"));
    QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("Power state"));
  }
  QCOMPARE(model->headerData(3, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(4, Qt::Horizontal), QVariant("Value"));
  // Check CSV headers
  csvHeader = manager.csvHeader();
  QCOMPARE(csvHeader.size(), 3);
  QCOMPARE(csvHeader.at(0), QString("bin I/O"));
  QCOMPARE(csvHeader.at(1), QString("signal"));
  QCOMPARE(csvHeader.at(2), QString("value"));

  return;

  /*
   * Playing ...
   */
  manager.clearFieldMap();
  manager.enableProgressDialog(true);
  manager.setCsvFormat(";", "\"", "", '"', "\r\n", "ISO-8859-15");
  ///void addFieldMapping(const QString &csvHeaderItem, const QString &fieldName, const QString &displayText, QVariant::Type dataType, int csvDataItemStartOffset = -1, int csvDataItemEndOffset = -1);
  manager.addFieldMapping("Logici 1", "digital1", "Logique 1", QVariant::String);
  manager.addFieldMapping("Logici 1", "fakeDJstate", "Fake DJ state", QVariant::String, 0, 0);
  manager.addFieldMapping("Logici 1", "fakeIRstate", "Fake IR state", QVariant::String, 1, 1);
  manager.addFieldMapping("Logici 1", "fakePanto1state", "Fake panto 1 state", QVariant::String, 2, 2);
  manager.addFieldMapping("Logici 1", "fakePanto2state", "Fake panto 2 state", QVariant::String, 3, 3);
  manager.addFieldMapping("Logici 1", "fakePanto3state", "Fake panto 3 state", QVariant::String, 4, 4);
  manager.addFieldMapping("Logici 1", "fakePanto4state", "Fake panto 4 state", QVariant::String, 5, 5);
  manager.addFieldMapping("Logici 1", "fakePanto5state", "Fake panto 5 state", QVariant::String, 6, 6);
  manager.addFieldMapping("Logici 1", "fakePanto6state", "Fake panto 6 state", QVariant::String, 7, 7);
  
  QVERIFY(manager.importFromCsvFile("/tmp/example_dwn.csv", mdtSqlDatabaseManager::AskUserIfExists));
  ///manager.setCsvFormat(";", "\"", "", '"', "\r\n", "UTF-8");
  ///QVERIFY(manager.importFromCsvFile("/tmp/example_unicontrol.csv", mdtDataTableManager::AskUserIfExists));
  ///QVERIFY(manager.importFromCsvFile("/media/PS_MBS/example_dwn.csv", mdtDataTableManager::AskUserIfExists));
  QVERIFY(manager.model() != 0);
  manager.setDisplayTextsToModelHeader();

  ///QString sql;
  ///QSqlQueryModel qm;

  ///qDebug() << "Available fields: " << manager.database().record(model->tableName());
  /**
  sql = "SELECT * FROM " + model->tableName() + ";";
  ///sql = "SELECT * id_PK, digital1, fakeDJstate FROM " + model->tableName() + ";";
  qDebug() << "SQL: " << sql;
  qm.setQuery(sql, manager.database());
  */
  
  // View
  QTableView v;
  v.setModel(manager.model());
  ///v.setModel(&qm);
  v.setEditTriggers(QAbstractItemView::NoEditTriggers);
  v.resize(600, 600);
  v.setSortingEnabled(true);
  v.horizontalHeader()->setMovable(true);
  v.show();

  
  mdtSqlQueryWidget qw;
  qw.setDatabase(manager.database());
  qw.setHeaderTextsByFieldNames(manager.displayTextsByFieldNames());
  qw.show();
  
  /**
  QTest::qWait(10000);
  sql = "SELECT id_PK, digital1, fakeDJstate FROM " + model->tableName() + ";";
  qDebug() << "SQL: " << sql;
  qm.setQuery(sql, manager.database());
  v.setModel(&qm);
  */

  
  while(v.isVisible()){
    QTest::qWait(1000);
  }

}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDataTableTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
