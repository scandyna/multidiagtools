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
#include "mdtDataTableModel.h"
#include "mdtApplication.h"
#include "mdtDataTableItemDelegate.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QSqlTableModel>
#include <QTableView>
#include <QModelIndex>
#include <QVariant>
#include <QTimer>
#include <QScrollBar>
#include <QPushButton>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QMap>

#include <QDebug>

void mdtDataTableTest::sandbox()
{
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
  ///mdtDataTableModel m;
  QTemporaryFile dbFile;
  QFileInfo fileInfo;
  QList<QSqlField> fields;
  QSqlField field;
  QSqlIndex pk;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;

  // Build fields
  pk.append(QSqlField("id_PK", QVariant::Int));
  field.setName("value");
  field.setType(QVariant::Double);
  ///field.setAutoValue(true);
  fields.append(field);
  field.setName("name");
  field.setType(QVariant::String);
  fields.append(field);

  /**
  QVERIFY(!m.setDataSetDirectory(QString("Ij98()'ç!?*%+")));
  QVERIFY(m.setDataSetDirectory(fileInfo.dir()));
  QVERIFY(m.setDataSetDirectory(fileInfo.dir().absolutePath()));
  qDebug() << "DB: " << fileInfo.fileName() << " , dir: " << fileInfo.dir().absolutePath();
  QVERIFY(m.dataSetDirectory() == fileInfo.dir());
  */
  // Check data set creation modes
  QVERIFY(dbFile.open());
  fileInfo.setFile(dbFile);
  dataSetName = fileInfo.fileName();
  dataSetTableName = mdtDataTableModel::getTableName(dataSetName);
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::OverwriteExisting);
  QVERIFY(db.isOpen());
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::KeepExisting);
  QVERIFY(db.isOpen());
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::FailIfExists);
  QVERIFY(!db.isOpen());
  // Check table creation
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::OverwriteExisting);
  QVERIFY(db.isOpen());
  /**
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::AskUserIfExists);
  QVERIFY(db.isOpen());
  */

  
  // Set model and check that columns exists
  mdtDataTableModel m(0, db);
  m.setTable(dataSetTableName);
  QCOMPARE(m.columnCount(), 3);
  QCOMPARE(m.headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(m.headerData(1, Qt::Horizontal), QVariant("value"));
  QCOMPARE(m.headerData(2, Qt::Horizontal), QVariant("name"));
  // Check that data set is empty
  QVERIFY(m.select());
  QCOMPARE(m.rowCount(), 0);
  // Insert some data
  ///QVERIFY(m.insertRows(0, 1));
  ///QVERIFY(, QVariant("A value")));
}

void mdtDataTableTest::editDataTest()
{
  QTemporaryFile dbFile;
  QFileInfo fileInfo;
  QList<QSqlField> fields;
  QSqlField field;
  QSqlIndex pk;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;
  QMap<QString,QVariant> rowData;

  // Build fields
  pk.append(QSqlField("id_PK", QVariant::Int));
  field.setName("signal");
  field.setType(QVariant::String);
  fields.append(field);
  field.setName("value");
  field.setType(QVariant::Double);
  fields.append(field);
  // Create data set
  QVERIFY(dbFile.open());
  fileInfo.setFile(dbFile);
  dataSetName = fileInfo.fileName();
  dataSetTableName = mdtDataTableModel::getTableName(dataSetName);
  db = mdtDataTableModel::createDataSet(fileInfo.dir(), dataSetName, pk, fields, mdtDataTableModel::OverwriteExisting);
  QVERIFY(db.isOpen());
  // Set model and check that columns exists
  mdtDataTableModel m(0, db);
  m.setTable(dataSetTableName);
  QCOMPARE(m.columnCount(), 3);
  QCOMPARE(m.headerData(0, Qt::Horizontal), QVariant("id_PK"));
  QCOMPARE(m.headerData(1, Qt::Horizontal), QVariant("signal"));
  QCOMPARE(m.headerData(2, Qt::Horizontal), QVariant("value"));
  // Check that data set is empty
  QVERIFY(m.select());
  QCOMPARE(m.rowCount(), 0);
  // Add a row with valid data - method A
  rowData.insert("signal", "Cmp. temp");
  rowData.insert("value", 25.4);
  QVERIFY(m.addRow(rowData));
  QCOMPARE(m.rowCount(), 1);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  // Add a row with to much fields - method A
  rowData.insert("fake", "??");
  QVERIFY(!m.addRow(rowData));
  QCOMPARE(m.rowCount(), 1);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  // Add a row with "to" less fields (must work) - method A
  rowData.clear();
  rowData.insert("signal", "Temp. M1");
  QVERIFY(m.addRow(rowData));
  QCOMPARE(m.rowCount(), 2);
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  // Add a row with wrong fields - method A
  rowData.clear();
  rowData.insert("wrongField", "????");
  QVERIFY(!m.addRow(rowData));
  QCOMPARE(m.rowCount(), 2);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  // Add a row with valid data - method B
  QVERIFY(m.insertRows(0, 1));
  QVERIFY(m.setData(0, 1, "V soll"));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  // Add a row with invalid row - method B
  QVERIFY(m.insertRows(0, 1));
  QVERIFY(!m.setData(15, 1, "Fake..."));
  m.revertRow(0);
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  // Add a row with invalid column - method B
  QVERIFY(m.insertRows(0, 1));
  QVERIFY(!m.setData(0, 15, "Fake..."));
  m.revertRow(0);
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  // Edit a row with valid data - method A
  QVERIFY(m.setData(2, 2, 56.4));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(56.4));
  // Edit a row with invalid row - method A
  QVERIFY(!m.setData(20, 2, 44.2));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(56.4));
  // Edit a row with invalid column - method A
  QVERIFY(!m.setData(2, 21, 42.5));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(56.4));
  // Edit a row with valid data - method B
  QVERIFY(m.setData(2, "value", 88.4));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(88.4));
  // Edit a row with invalid row - method B
  QVERIFY(!m.setData(25, "value", 78));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(88.4));
  // Edit a row with invalid column - method B
  QVERIFY(!m.setData(2, "fake..", 68.2));
  QCOMPARE(m.rowCount(), 3);
  QCOMPARE(m.data(m.index(0, 1)), QVariant("Cmp. temp"));
  QCOMPARE(m.data(m.index(0, 2)), QVariant(25.4));
  QCOMPARE(m.data(m.index(1, 1)), QVariant("Temp. M1"));
  QCOMPARE(m.data(m.index(2, 1)), QVariant("V soll"));
  QCOMPARE(m.data(m.index(2, 2)), QVariant(88.4));
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
