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
#include "mdtSqlQueryWidget.h"
#include "mdtSqlSchemaTable.h"
#include <QSqlDatabase>
#include <QSqlQuery>
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
  ///QList<QSqlField> fields;
  QSqlField field;
  ///QSqlIndex pk;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;
  
  mdtSqlSchemaTable table;

  // Build fields
  /**
  pk.append(QSqlField("id_PK", QVariant::Int));
  field.setName("value");
  field.setType(QVariant::Double);
  fields.append(field);
  field.setName("name");
  field.setType(QVariant::String);
  fields.append(field);
  */
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
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.database().isOpen());
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::KeepExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::KeepExisting));
  QVERIFY(manager.database().isOpen());
  ///QVERIFY(!manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::FailIfExists));
  QVERIFY(!manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::FailIfExists));
  QVERIFY(!manager.database().isOpen());

  // Check table creation: PK fields are automatically created
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
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

  // Check table creation: PK fields are NOT automatically created
  ///QVERIFY(!manager.createDataSet(fileInfo.dir(), dataSetName, pk, false, fields, mdtDataTableManager::OverwriteExisting));
  ///QVERIFY(!manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
  ///QVERIFY(!manager.database().isOpen());
  // Rebuild fields in correct way
  /**
  fields.clear();
  field.setName("id_PK");
  field.setType(QVariant::Int);
  fields.append(field);
  field.setName("value");
  field.setType(QVariant::Double);
  fields.append(field);
  field.setName("name");
  field.setType(QVariant::String);
  fields.append(field);
  */
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, false, fields, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
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
  ///QList<QSqlField> fields;
  QSqlField field;
  ///QSqlIndex pk;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;
  QMap<QString,QVariant> rowData;
  QList<QVariant> rowData2;
  mdtFieldMap fieldMap;
  mdtFieldMapItem *mapItem;
  QStringList rowData3;
  ///QList<QVariant> rowData3;
  QList<QStringList> rowsData4;
  ///QList<QList<QVariant> > rowsData4;

  mdtSqlSchemaTable table;
  
  // Build fields
  /**
  pk.append(QSqlField("id_PK", QVariant::Int));
  field.setName("signal");
  field.setType(QVariant::String);
  fields.append(field);
  field.setName("value");
  field.setType(QVariant::Double);
  fields.append(field);
  */
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
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
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
  mapItem->setFieldIndex(0);
  mapItem->setFieldName("id_PK");
  mapItem->setSourceFieldIndex(-1);
  mapItem->setSourceFieldName("id_PK");
  mapItem->setDataType(QVariant::Int);
  fieldMap.addItem(mapItem);
  mapItem = new mdtFieldMapItem;
  mapItem->setFieldIndex(1);
  mapItem->setFieldName("signal");
  mapItem->setSourceFieldIndex(0);
  mapItem->setSourceFieldName("signal");
  mapItem->setDataType(QVariant::String);
  fieldMap.addItem(mapItem);
  mapItem = new mdtFieldMapItem;
  mapItem->setFieldIndex(2);
  mapItem->setFieldName("value");
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
  item->setFieldIndex(0);
  item->setFieldName("A");
  item->setFieldDisplayText("A");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(1);
  item->setSourceFieldName("B");
  item->setFieldIndex(1);
  item->setFieldName("B");
  item->setFieldDisplayText("B");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(2);
  item->setSourceFieldName("Group1");
  item->setSourceFieldDataStartOffset(0);
  item->setSourceFieldDataEndOffset(1);
  item->setFieldIndex(2);
  item->setFieldName("Sub1Id");
  item->setFieldDisplayText("Grp 1 ID");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(2);
  item->setSourceFieldName("Group1");
  item->setSourceFieldDataStartOffset(2);
  item->setSourceFieldDataEndOffset(-1);
  item->setFieldIndex(3);
  item->setFieldName("Sub1Name");
  item->setFieldDisplayText("Grp 1 Name");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(3);
  item->setSourceFieldName("C");
  item->setFieldIndex(4);
  item->setFieldName("C");
  item->setFieldDisplayText("C");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(0);
  item->setSourceFieldDataEndOffset(2);
  item->setFieldIndex(5);
  item->setFieldName("Sub2Id");
  item->setFieldDisplayText("Grp 2 ID");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(3);
  item->setSourceFieldDataEndOffset(7);
  item->setFieldIndex(6);
  item->setFieldName("Sub2Name");
  item->setFieldDisplayText("Grp 2 Name");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldIndex(4);
  item->setSourceFieldName("Group2");
  item->setSourceFieldDataStartOffset(8);
  item->setSourceFieldDataEndOffset(15);
  item->setFieldIndex(7);
  item->setFieldName("Sub2Value");
  item->setFieldDisplayText("Grp 2 Value");
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
    QCOMPARE(item->fieldIndex(), i);
    QCOMPARE(item->fieldName(), modelHeader.at(i));
    item = map.itemAtFieldName(modelHeader.at(i));
    QVERIFY(item != 0);
    QCOMPARE(item->fieldIndex(), i);
    QCOMPARE(item->fieldName(), modelHeader.at(i));
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
  QCOMPARE(item->fieldIndex(), 2);
  QCOMPARE(item->fieldName(), QString("Sub1Id"));
  QCOMPARE(item->fieldDisplayText(), QString("Grp 1 ID"));
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("Group1"));
  QCOMPARE(item->sourceFieldDataStartOffset(), 0);
  QCOMPARE(item->sourceFieldDataEndOffset(), 1);
  item = map.itemAtFieldName("Sub1Name");
  QVERIFY(item != 0);
  QCOMPARE(item->fieldIndex(), 3);
  QCOMPARE(item->fieldName(), QString("Sub1Name"));
  QCOMPARE(item->fieldDisplayText(), QString("Grp 1 Name"));
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("Group1"));
  // Check Group2 split
  item = map.itemAtFieldName("Sub2Id");
  QVERIFY(item != 0);
  QCOMPARE(item->fieldIndex(), 5);
  QCOMPARE(item->fieldName(), QString("Sub2Id"));
  QCOMPARE(item->fieldDisplayText(), QString("Grp 2 ID"));
  QCOMPARE(item->sourceFieldIndex(), 4);
  QCOMPARE(item->sourceFieldName(), QString("Group2"));
  item = map.itemAtFieldName("Sub2Name");
  QVERIFY(item != 0);
  QCOMPARE(item->fieldIndex(), 6);
  QCOMPARE(item->fieldName(), QString("Sub2Name"));
  QCOMPARE(item->fieldDisplayText(), QString("Grp 2 Name"));
  QCOMPARE(item->sourceFieldIndex(), 4);
  QCOMPARE(item->sourceFieldName(), QString("Group2"));
  item = map.itemAtFieldName("Sub2Value");
  QVERIFY(item != 0);
  QCOMPARE(item->fieldIndex(), 7);
  QCOMPARE(item->fieldName(), QString("Sub2Value"));
  QCOMPARE(item->fieldDisplayText(), QString("Grp 2 Value"));
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
  item->setFieldName("a1");
  item->setFieldDisplayText("A 1");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("A");
  item->setFieldName("a2");
  item->setFieldDisplayText("A 2");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("B");
  item->setFieldName("b");
  item->setFieldDisplayText("B");
  item->setDataType(QVariant::String);
  map.addItem(item);
  item = new mdtFieldMapItem;
  item->setSourceFieldName("C");
  item->setFieldName("c");
  item->setFieldDisplayText("C");
  item->setDataType(QVariant::String);
  map.addItem(item);
  // Check the mapping
  item = map.itemAtFieldName("a1");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->fieldName(), QString("a1"));
  QCOMPARE(item->fieldDisplayText(), QString("A 1"));
  item = map.itemAtFieldName("a2");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->fieldName(), QString("a2"));
  QCOMPARE(item->fieldDisplayText(), QString("A 2"));
  item = map.itemAtFieldName("b");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->fieldName(), QString("b"));
  QCOMPARE(item->fieldDisplayText(), QString("B"));
  item = map.itemAtFieldName("c");
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->fieldName(), QString("c"));
  QCOMPARE(item->fieldDisplayText(), QString("C"));
  // Update indexes
  item = map.itemAtFieldName("a1");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(1);
  item->setFieldIndex(10);
  map.updateItem(item);
  item = map.itemAtFieldName("a2");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(1);
  item->setFieldIndex(11);
  map.updateItem(item);
  item = map.itemAtFieldName("b");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(2);
  item->setFieldIndex(12);
  map.updateItem(item);
  item = map.itemAtFieldName("c");
  QVERIFY(item != 0);
  item->setSourceFieldIndex(3);
  item->setFieldIndex(13);
  map.updateItem(item);
  // Check the mapping - search by field index
  item = map.itemAtFieldIndex(10);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->fieldIndex(), 10);
  QCOMPARE(item->fieldName(), QString("a1"));
  item = map.itemAtFieldIndex(11);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  QCOMPARE(item->fieldIndex(), 11);
  QCOMPARE(item->fieldName(), QString("a2"));
  item = map.itemAtFieldIndex(12);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->fieldIndex(), 12);
  QCOMPARE(item->fieldName(), QString("b"));
  item = map.itemAtFieldIndex(13);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 3);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->fieldIndex(), 13);
  QCOMPARE(item->fieldName(), QString("c"));
  // Check the mapping - search by source field index
  items = map.itemsAtSourceFieldIndex(1);
  QVERIFY(items.size() == 2);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 1);
  QCOMPARE(item->sourceFieldName(), QString("A"));
  // We don't know how items are ordered by field index
  if(item->fieldIndex() == 10){
    QCOMPARE(item->fieldName(), QString("a1"));
    item = items.at(1);
    QVERIFY(item != 0);
    QCOMPARE(item->sourceFieldIndex(), 1);
    QCOMPARE(item->sourceFieldName(), QString("A"));
    QCOMPARE(item->fieldIndex(), 11);
    QCOMPARE(item->fieldName(), QString("a2"));
  }else if(item->fieldIndex() == 11){
    QCOMPARE(item->fieldName(), QString("a2"));
    item = items.at(1);
    QVERIFY(item != 0);
    QCOMPARE(item->sourceFieldIndex(), 1);
    QCOMPARE(item->sourceFieldName(), QString("A"));
    QCOMPARE(item->fieldIndex(), 10);
    QCOMPARE(item->fieldName(), QString("a1"));

  }
  items = map.itemsAtSourceFieldIndex(2);
  QVERIFY(items.size() == 1);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 2);
  QCOMPARE(item->sourceFieldName(), QString("B"));
  QCOMPARE(item->fieldIndex(), 12);
  QCOMPARE(item->fieldName(), QString("b"));
  items = map.itemsAtSourceFieldIndex(3);
  QVERIFY(items.size() == 1);
  item = items.at(0);
  QVERIFY(item != 0);
  QCOMPARE(item->sourceFieldIndex(), 3);
  QCOMPARE(item->sourceFieldName(), QString("C"));
  QCOMPARE(item->fieldIndex(), 13);
  QCOMPARE(item->fieldName(), QString("c"));
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
  ///QList<QSqlField> fields;
  QSqlField field;
  ///QSqlIndex pk;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;
  QMap<QString,QVariant> rowData;
  
  mdtSqlSchemaTable table;

  // Build fields
  /**
  pk.append(QSqlField("id_PK", QVariant::Int));
  field.setName("signal");
  field.setType(QVariant::String);
  fields.append(field);
  field.setName("value");
  field.setType(QVariant::Double);
  fields.append(field);
  */
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
  ///QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, pk, true, fields, mdtDataTableManager::OverwriteExisting));
  QVERIFY(manager.createDataSet(fileInfo.dir(), dataSetName, table, mdtDataTableManager::OverwriteExisting));
  // Set model and check that columns exists
  mdtDataTableModel m(0, db);
  m.setTable(dataSetTableName);
  QVERIFY(m.select());
  QCOMPARE(m.columnCount(), 3);
  QCOMPARE(m.headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(m.headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(m.headerData(2, Qt::Horizontal), QVariant("value"));

}

void mdtDataTableTest::csvImportTest()
{
  QTemporaryFile csvFile1, csvFile2, csvFile3;
  mdtDataTableManager manager;
  mdtDataTableModel *model;
  QStringList csvHeader;

  // Write CSV file that contains a primary key
  QVERIFY(csvFile1.open());
  QVERIFY(csvFile1.write("id_PK;signal;value" + QByteArray(MDT_NATIVE_EOL)) > 0);
  QVERIFY(csvFile1.write("1;Temp. M1;125.0" + QByteArray(MDT_NATIVE_EOL)) > 0);
  csvFile1.close();
  // Configure manager and import CSV
  manager.setCsvFormat(";", "", "", '\0');
  QVERIFY(manager.importFromCsvFile(csvFile1.fileName(), mdtDataTableManager::OverwriteExisting, "", QStringList("id_PK")));
  model = manager.model();
  QVERIFY(model != 0);
  // Check model's header
  QCOMPARE(model->headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(model->headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(model->headerData(2, Qt::Horizontal), QVariant("value"));
  // Check CSV headers
  csvHeader = manager.csvHeader();
  QCOMPARE(csvHeader.size(), 3);
  QCOMPARE(csvHeader.at(0), QString("id_PK"));
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
  QVERIFY(manager.importFromCsvFile(csvFile2.fileName(), mdtDataTableManager::OverwriteExisting));
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
  QVERIFY(manager.importFromCsvFile(csvFile3.fileName(), mdtDataTableManager::OverwriteExisting));
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
  if(model->headerData(1, Qt::Horizontal) == QVariant("powerState")){
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
  
  QVERIFY(manager.importFromCsvFile("/tmp/example_dwn.csv", mdtDataTableManager::AskUserIfExists));
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
