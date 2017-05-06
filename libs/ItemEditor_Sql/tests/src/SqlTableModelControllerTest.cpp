/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "SqlTableModelControllerTest.h"
#include "SqlTableModelControllerTester.h"
#include "RowStateChangedSpy.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/ItemEditor/RowState.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QPointer>
#include <QScopedPointer>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;
namespace Sql = Mdt::Sql;
using Schema::Client;
using Schema::Address;
using Schema::ClientAddressSchema;
using Schema::ClientPopulation;

void SqlTableModelControllerTest::initTestCase()
{
  if(!initDatabaseSqlite()){
    QSKIP("Could not init database - Skip all tests");  // Will also skip all tests
  }
  createSchema(ClientAddressSchema());
}

void SqlTableModelControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SqlTableModelControllerTest::setModelTest()
{
  /*
   * Initial state
   */
  SqlTableModelControllerTester controller;
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.sourceModel() == nullptr);
  /*
   * Set a empty model
   */
  QSqlTableModel model1;
  controller.setModel(&model1);
  QCOMPARE(controller.model(), &model1);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.model()->editStrategy(), QSqlTableModel::OnManualSubmit);
}

void SqlTableModelControllerTest::setDefaultModelTest()
{
  /*
   * Initial state
   */
  SqlTableModelControllerTester controller;
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.sourceModel() == nullptr);
  /*
   * Set default model
   */
  controller.setDefaultModel(database());
  QVERIFY(controller.model() != nullptr);
  QPointer<QSqlTableModel> defaultModel = controller.model();
  QVERIFY(!defaultModel.isNull());
  QCOMPARE(controller.sourceModel(), defaultModel.data());
  QCOMPARE(controller.model()->database().databaseName(), database().databaseName());
  QCOMPARE(controller.model()->editStrategy(), QSqlTableModel::OnManualSubmit);
  /*
   * Set a other model
   * Default model must also be deleted
   */
  QPointer<QSqlTableModel> model(new QSqlTableModel);
  controller.setModel(model);
  QVERIFY(defaultModel.isNull());
  QVERIFY(!model.isNull());
  QCOMPARE(controller.model(), model.data());
  QCOMPARE(controller.sourceModel(), model.data());
  QCOMPARE(controller.model()->editStrategy(), QSqlTableModel::OnManualSubmit);
  /*
   * Set default model again
   * Our model must not be deleted
   */
  controller.setDefaultModel(database());
  QVERIFY(!model.isNull());
  delete model;
  QVERIFY(controller.model() != nullptr);
  QCOMPARE(controller.model()->database().databaseName(), database().databaseName());
  QCOMPARE(controller.model()->editStrategy(), QSqlTableModel::OnManualSubmit);
  /*
   * Set default model again that acts on a other db connection
   */
  auto db = QSqlDatabase::addDatabase("QSQLITE", "CNN2");
  QVERIFY(db.isValid());
  db.setDatabaseName("Other");
  controller.setDefaultModel(db);
  QCOMPARE(controller.model()->database().databaseName(), db.databaseName());
  QCOMPARE(controller.model()->editStrategy(), QSqlTableModel::OnManualSubmit);
}

void SqlTableModelControllerTest::setTableTest()
{
  SqlTableModelControllerTester controller;
  Schema::Client client;

  controller.setDefaultModel(database());
  QVERIFY(controller.model() != nullptr);
  /*
   * Check setting with schema table
   */
  controller.setTable(client.toTable());
  QCOMPARE(controller.model()->tableName(), QString("Client_tbl"));
  /*
   * Set table name
   */
  controller.setTableName("A_tbl");
  QCOMPARE(controller.model()->tableName(), QString("A_tbl"));
  /*
   * Check setting with table template
   */
  controller.setTable(client);
  QCOMPARE(controller.model()->tableName(), QString("Client_tbl"));
}

void SqlTableModelControllerTest::setTablePkFkTest()
{

  QFAIL("Not complete");
}

void SqlTableModelControllerTest::selectTest()
{
  SqlTableModelControllerTester controller;
  Schema::Client client;
  RowStateChangedSpy rowStateSpy(controller);
  RowState rs;
  /*
   * Prepare client data
   */
  ClientPopulation cp;
  cp.addClient("A");
  cp.addClient("B");
  QVERIFY(repopulateClientTable(cp));
  /*
   * Setup controller
   */
  controller.setDefaultModel(database());
  controller.setTable(client);
  rowStateSpy.clear();
  /*
   * Select
   */
  QVERIFY(controller.select());
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  // Check that model shows the data
  QCOMPARE(controller.currentData(1), QVariant("A"));
  /*
   * Go to row 1
   */
  QVERIFY(controller.setCurrentRow(1));
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 1);
  // Check that model shows the data
  QCOMPARE(controller.currentData(1), QVariant("B"));
}

void SqlTableModelControllerTest::removeTest()
{
  QFETCH(QStringList, initialList);
  QFETCH(RowList, rowList);
  QFETCH(QStringList, expectedList);
  SqlTableModelControllerTester controller;
  Schema::Client client;
  /*
   * Prepare client data
   */
  ClientPopulation cp;
  for(const auto name : initialList){
    cp.addClient(name);
  }
  QVERIFY(repopulateClientTable(cp));
  /*
   * Setup controller
   */
  controller.setDefaultModel(database());
  controller.setTable(client);
  QVERIFY(controller.select());
  QCOMPARE(controller.rowCount(), initialList.size());
  /*
   * Check
   */
  controller.setSelectedRows(rowList);
  QVERIFY(controller.remove());
  const auto *model = controller.model();
  QCOMPARE(model->rowCount(), expectedList.count());
  for(int row = 0; row < expectedList.count(); ++row){
    QCOMPARE(getModelData(model, row, 1).toString(), expectedList.at(row));
  }
}

void SqlTableModelControllerTest::removeTest_data()
{
  QTest::addColumn<QStringList>("initialList");
  QTest::addColumn<RowList>("rowList");
  QTest::addColumn<QStringList>("expectedList");

  QTest::newRow("{0}") << QStringList{"A","B","C"} << RowList{0} << QStringList{"B","C"};
  QTest::newRow("{0,1}") << QStringList{"A","B","C"} << RowList{0,1} << QStringList{"C"};
  QTest::newRow("{0,2}") << QStringList{"A","B","C"} << RowList{0,2} << QStringList{"B"};
  QTest::newRow("{0,1,3,4}") << QStringList{"A","B","C","D","E"} << RowList{0,1,3,4} << QStringList{"C"};
}

void SqlTableModelControllerTest::editSubmitTest()
{

  /*
   * Prepare client data
   */
  ClientPopulation cp;
  cp.addClient(1, "A");
  cp.addClient(2, "B");
  QVERIFY(repopulateClientTable(cp));
  

  qDebug() << getClientNameFromDatabase(1);
  qDebug() << getClientNameFromDatabase(2);
  
  QFAIL("Not complete");
}

void SqlTableModelControllerTest::editSelectTest()
{

  QFAIL("Not complete");
}

/*
 * Helpers
 */

void SqlTableModelControllerTest::createSchema(const Schema::ClientAddressSchema& schema)
{
  Sql::Schema::Driver driver(database());

  QVERIFY(driver.isValid());
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(driver.createSchema(schema));
}

bool SqlTableModelControllerTest::deleteClientData()
{
  QSqlQuery query(database());
  return query.exec("DELETE FROM Client_tbl");
}

bool SqlTableModelControllerTest::repopulateClientTable(const Schema::ClientPopulation& tp)
{
  Sql::Schema::Driver driver(database());

  Q_ASSERT(driver.isValid());
  if(!deleteClientData()){
    return false;
  }
  return driver.populateTable(tp);
}

QVariant SqlTableModelControllerTest::getClientNameFromDatabase(int id)
{
  Schema::Client client;
  const QString sql = "SELECT " + client.Name().name() + " FROM " + client.tableName() \
                    + " WHERE " + client.Id_PK().fieldName() + " =" + QString::number(id);
  QSqlQuery query(database());
  if(!query.exec(sql)){
    qDebug() << query.lastError();
    return QVariant();
  }
  query.next();

  return query.value(0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SqlTableModelControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
