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
#include "mdtCableListTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClArticle.h"
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

void mdtCableListTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtCableListTest::cleanupTestCase()
{
}

void mdtCableListTest::articleTest()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  mdtSqlRecord connectorData, connectionData;
  bool ok;

  /*
   * Simple article manipulation
   */

  // Add a article
  QVERIFY(record.addAllFields("Article_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("ArticleCode", "1234");
  record.setValue("DesignationEN", "Article 1234");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check added article
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("ArticleCode"), QVariant("1234"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 1234"));
  // Add a article
  record.clearValues();
  record.setValue("Id_PK", 2);
  record.setValue("ArticleCode", "ABC");
  record.setValue("DesignationEN", "Article ABC");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check existing articles
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("ArticleCode"), QVariant("1234"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 1234"));
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("ArticleCode"), QVariant("ABC"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article ABC"));

  /*
   * Article component
   */

  // Initially we have not assigned any component
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Assign article ID 2 as component of article ID 1
  QVERIFY(art.addComponent(1, 2, 5, "pce"));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Article_Id_PK"), QVariant(1));
  QCOMPARE(data.value("Component_Id_PK"), QVariant(2));
  QCOMPARE(data.value("ComponentQty"), QVariant(5));
  QCOMPARE(data.value("ComponentQtyUnit"), QVariant("pce"));
  // Try to remove wrong component assignation - Note: possibly ok that no error is returned
  art.removeComponent(5, 3);
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove component assignation
  QVERIFY(art.removeComponent(1, 2));
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Article connection
   */

  record.clear();
  QVERIFY(record.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
  // Initially we have not connections
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Add a connection to article ID 1
  record.setValue("Id_PK", 1);
  record.setValue("Article_Id_FK", 1);
  record.setValue("ArticleContactName", "Contact 1");
  record.setValue("FunctionEN", "EN: Contact 1");
  QVERIFY(art.addConnection(record));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ArticleContactName"), QVariant("Contact 1"));
  QVERIFY(data.value("ArticleConnector_Id_FK").isNull());
  QCOMPARE(data.value("FunctionEN"), QVariant("EN: Contact 1"));
  // Try to delete article ID 1 - Must fail
  QVERIFY(!art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  // Remove connection
  QVERIFY(art.removeConnection(1));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Article connector + connections
   */

  // Setup connector data for article ID 1
  connectorData.clear();
  QVERIFY(connectorData.addAllFields("ArticleConnector_tbl", pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 1);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Name", "X1");
  // Setup connection data
  connectionData.clear();
  QVERIFY(connectionData.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
  for(int i = 0; i < 11; ++i){
    connectionData.clearValues();
    connectionData.setValue("ArticleContactName", QString::number(i));
    connectionData.setValue("FunctionEN", "Contact " + QString::number(i));
    dataList.append(connectionData);
  }
  // Add connector
  QVERIFY(art.addConnector(connectorData, dataList));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 11);
  for(int i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
    QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(1));
    QCOMPARE(data.value("ArticleConnectorName"), QVariant("X1"));
    QCOMPARE(data.value("ArticleContactName"), QVariant(QString::number(i)));
    QCOMPARE(data.value("FunctionEN"), QVariant("Contact " + QString::number(i)));
  }

  /*
   * Article link
   */
}

/*
 * Test database helper methods
 */

void mdtCableListTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  dbFileInfo.setFile(dbFile.fileName() + ".db");
  mdtTtDatabaseSchema schema(&pvDatabaseManager);
  QVERIFY(schema.createSchemaSqlite(dbFileInfo));
  QVERIFY(pvDatabaseManager.database().isOpen());
  QVERIFY(schema.checkSchema());
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtCableListTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
