/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtDatabaseWidgetTest.h"
#include "mdtSqlSchemaTable.h"
#include "mdtApplication.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>
#include <QList>
#include <QFileInfo>

#include <QTableView>
#include <QItemSelectionModel>

#include <QDebug>

void mdtDatabaseWidgetTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
  /**
  populateTestDatabase();
  clearTestDatabaseData();
  */
}

void mdtDatabaseWidgetTest::cleanupTestCase()
{
  QFile::remove(pvDatabaseManager.database().databaseName());
}

/*
 * Helper methods
 */

void mdtDatabaseWidgetTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;
  mdtSqlSchemaTable st;
  QSqlField field;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  dbFileInfo.setFile(dbFile.fileName() + ".db");
  QVERIFY(pvDatabaseManager.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting));
  /*
   * Create schema
   */
  // Client_tbl
  st.clear();
  st.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  // FirstName
  field = QSqlField();
  field.setName("FirstName");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // Remarks
  field = QSqlField();
  field.setName("Remarks");
  field.setType(QVariant::String);
  field.setLength(100);
  st.addField(field, false);
  QVERIFY(pvDatabaseManager.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));
  // Address_tbl
  st.clear();
  st.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  // StreetName
  field = QSqlField();
  field.setName("StreetName");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // StreetNumber
  field = QSqlField();
  field.setName("StreetNumber");
  field.setType(QVariant::Int);
  st.addField(field, false);
  // Client_Id_FK
  field = QSqlField();
  field.setName("Client_Id_FK");
  field.setType(QVariant::Int);
  st.addField(field, false);
  st.addForeignKey("Client_Id_FK_fk", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK_fk", "Client_Id_FK", "Id_PK"));
  QVERIFY(pvDatabaseManager.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));

  // Enable foreing keys support
  QVERIFY(pvDatabaseManager.setForeignKeysEnabled(true));
}

void mdtDatabaseWidgetTest::populateTestDatabase()
{
  QSqlQuery query(pvDatabaseManager.database());

  // Inert some data in Client_tbl
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, FirstName) VALUES(1, 'Andy')"));
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(2, 'Bety', 'Remark on Bety')"));
  QVERIFY(query.exec("SELECT * FROM 'Client_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
  // Inert some data into Address_tbl
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 1', 11 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 2', 12 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Bety street 1', 21 , 2)"));
  QVERIFY(query.exec("SELECT * FROM 'Address_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
}

void mdtDatabaseWidgetTest::clearTestDatabaseData()
{
  QString sql;
  QSqlQuery query(pvDatabaseManager.database());

  sql = "DELETE FROM Address_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM Client_tbl";
  QVERIFY(query.exec(sql));
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDatabaseWidgetTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
