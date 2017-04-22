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
#include "SqlTableViewControllerTest.h"
#include "RowStateChangedSpy.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/ItemEditor/SqlTableViewController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QSqlQuery>
#include <QSqlError>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;
namespace Sql = Mdt::Sql;
using Schema::Client;
using Schema::Address;
using Schema::ClientAddressSchema;
using Schema::ClientPopulation;

void SqlTableViewControllerTest::initTestCase()
{
  if(!initDatabaseSqlite()){
    QSKIP("Could not init database - Skip all tests");  // Will also skip all tests
  }
  createSchema(ClientAddressSchema());
//   // Get database instance
//   mDatabase = QSqlDatabase::addDatabase("QSQLITE");
//   if(!mDatabase.isValid()){
//     QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
//   }
//   // Create a database
//   QVERIFY(mTempFile.open());
//   mTempFile.close();
//   mDatabase.setDatabaseName(mTempFile.fileName());
//   QVERIFY(mDatabase.open());
//   QSqlQuery query(mDatabase);
//   // Specify encoding (is important for some tests)
//   QVERIFY(query.exec("PRAGMA encoding = \"UTF-8\""));
//   // Create default schema
//   createSchema(ClientAddressSchema());
}

void SqlTableViewControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SqlTableViewControllerTest::setModelThenViewTest()
{
  QTableView tableView;
  SqlTableViewController controller;
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
   * Setup model
   */
  QSqlTableModel model(nullptr, database());
  model.setTable(Client().tableName());
  QVERIFY(model.select());
  QCOMPARE(model.rowCount(), 2);
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  controller.setModel(&model);
  QVERIFY(controller.model() == &model);
  // Check that row state was not signaled (no view is attached)
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Affect view to controller
   */
  controller.setView(&tableView);
  QCOMPARE(controller.view(), &tableView);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.currentRow(), 0);
}

void SqlTableViewControllerTest::setViewThenModelTest()
{
  QTableView tableView;
  SqlTableViewController controller;
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
   * Setup model
   */
  QSqlTableModel model(nullptr, database());
  model.setTable(Client().tableName());
  QVERIFY(model.select());
  QCOMPARE(model.rowCount(), 2);
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Affect view to controller
   */
  controller.setView(&tableView);
  QCOMPARE(controller.view(), &tableView);
  // Check that row state was not signaled (no model was set)
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  controller.setModel(&model);
  QVERIFY(controller.model() == &model);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.currentRow(), 0);
}

// void SqlTableViewControllerTest::setDefaultModelThenViewTest()
// {
//   QTableView tableView;
//   SqlTableViewController controller;
//   QSignalSpy rowStateSpy(&controller, &SqlTableViewController::rowStateChanged);
//   QList<QVariant> spyItem;
//   RowState rs;
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Initial state
//    */
//   QVERIFY(controller.model() == nullptr);
//   QVERIFY(controller.view() == nullptr);
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Set default model
//    */
//   controller.setDefaultModel(database());
//   QPointer<QSqlTableModel> defaultModel = controller.model();
//   QVERIFY(!defaultModel.isNull());
//   // Check that row state was not signaled (no view is attached)
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Affect view to controller
//    */
//   controller.setView(&tableView);
//   QCOMPARE(controller.view(), &tableView);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), 0);
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Set a other model and check that default one is deleted
//    */
//   QVERIFY(!defaultModel.isNull());
//   QSqlTableModel model;
//   controller.setModel(&model);
//   QVERIFY(defaultModel.isNull());
// }

// void SqlTableViewControllerTest::setViewThenDefaultModelTest()
// {
// //   using ItemEditor::RowState;
// 
//   QTableView tableView;
//   SqlTableViewController controller;
//   QSignalSpy rowStateSpy(&controller, &SqlTableViewController::rowStateChanged);
//   QList<QVariant> spyItem;
//   RowState rs;
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Initial state
//    */
//   QVERIFY(controller.model() == nullptr);
//   QVERIFY(controller.view() == nullptr);
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Affect view to controller
//    */
//   controller.setView(&tableView);
//   QCOMPARE(controller.view(), &tableView);
//   // Check that row state was not signaled (no model was set)
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Set default model
//    */
//   controller.setDefaultModel(database());
//   QVERIFY(controller.model() != nullptr);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), 0);
//   QCOMPARE(rs.currentRow(), -1);
// }

// void SqlTableViewControllerTest::changeModelTest()
// {
// //   using ItemEditor::RowState;
// 
//   QTableView tableView;
//   SqlTableViewController controller;
//   QSignalSpy rowStateSpy(&controller, &SqlTableViewController::rowStateChanged);
//   QList<QVariant> spyItem;
//   RowState rs;
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Prepare client data
//    */
//   ClientPopulation cp;
//   cp.addClient("A");
//   cp.addClient("B");
//   QVERIFY(repopulateClientTable(cp));
//   /*
//    * Setup models
//    */
//   QSqlTableModel emptyModel(nullptr, database());
//   QSqlTableModel clientModel(nullptr, database());
//   clientModel.setTable("Client_tbl");
//   QVERIFY(clientModel.select());
//   QCOMPARE(clientModel.rowCount(), 2);
//   /*
//    * Affect view to controller
//    */
//   controller.setView(&tableView);
//   QCOMPARE(controller.view(), &tableView);
//   // Check that row state was not signaled (no model was set)
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Set empty model
//    */
//   controller.setModel(&emptyModel);
//   QVERIFY(controller.model() == &emptyModel);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), 0);
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Set client model
//    */
//   controller.setModel(&clientModel);
//   QVERIFY(controller.model() == &clientModel);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), 2);
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Play
//    */
// //   tableView.show();
// //   while(tableView.isVisible()){
// //     QTest::qWait(500);
// //   }
// }

// void SqlTableViewControllerTest::changeDefaultModelTest()
// {
// 
//   /*
//    * Set default model
//    */
//   
//   /*
//    * Change database
//    */
//   
//   QFAIL("Not complete");
// }

// void SqlTableViewControllerTest::setTableTest()
// {
//   QTableView tableView;
//   SqlTableViewController controller;
//   Schema::Client client;
// 
//   controller.setDefaultModel(mDatabase);
//   QVERIFY(controller.model() != nullptr);
//   /*
//    * Check setting with schema table
//    */
//   controller.setTable(client.toTable());
//   QCOMPARE(controller.model()->tableName(), QString("Client_tbl"));
//   /*
//    * Set table name
//    */
//   controller.setTableName("A_tbl");
//   QCOMPARE(controller.model()->tableName(), QString("A_tbl"));
//   /*
//    * Check setting with table template
//    */
//   controller.setTable(client);
//   QCOMPARE(controller.model()->tableName(), QString("Client_tbl"));
// }

void SqlTableViewControllerTest::selectTest()
{
  Schema::Client client;
  QTableView tableView;
  SqlTableViewController controller;
  QSignalSpy rowStateSpy(&controller, &SqlTableViewController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
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
  controller.setView(&tableView);
  tableView.show();
  rowStateSpy.clear();
  /*
   * Select
   */
  QVERIFY(controller.select());
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  /**
   * \todo Hmm... Submit method ??
   */
  /*
   * Try to select while editing
   */
  // Begin editing
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  beginEditing(tableView, 0, 0, BeginEditTrigger::DoubleClick);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that we cannot select
  QVERIFY(!controller.select());
  QCOMPARE(rowStateSpy.count(), 0);
  // End editing
  endEditing(tableView, 0, 0, EndEditTrigger::EnterKeyClick);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  /*
   * Select
   */
  QVERIFY(controller.select());
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
}

void SqlTableViewControllerTest::currentRowChangeTest()
{
  QFAIL("Not complete");
}

void SqlTableViewControllerTest::stateChangeSignalTest()
{
  QFAIL("Not complete");
}

void SqlTableViewControllerTest::editSubmitRevertTest()
{
  Schema::Client client;
  QTableView tableView;
  SqlTableViewController controller;
  /*
   * Prepare client data
   */
  ClientPopulation cp;
  cp.addClient(1, "A");
  cp.addClient(2, "B");
  QVERIFY(repopulateClientTable(cp));
  /*
   * Setup controller
   */
  controller.setDefaultModel(database());
  controller.setTable(client);
  controller.setView(&tableView);
  QVERIFY(controller.select());
  QCOMPARE(controller.currentData(1), QVariant("A"));
  tableView.show();
  /*
   * Edit
   */
  edit(tableView, 0, 1, "AE", BeginEditTrigger::F2KeyClick, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.currentData(1), QVariant("AE"));
  // Check that database is not touched now
  QCOMPARE(getClientNameFromDatabase(1), QVariant("A"));
  /*
   * Submit
   */
  QVERIFY(controller.submit());
  QCOMPARE(controller.currentData(1), QVariant("AE"));
  QCOMPARE(getClientNameFromDatabase(1), QVariant("AE"));
  /*
   * Edit
   */
  edit(tableView, 0, 1, "AR", BeginEditTrigger::F2KeyClick, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.currentData(1), QVariant("AR"));
  // Check that database is not touched now
  QCOMPARE(getClientNameFromDatabase(1), QVariant("AE"));
  /*
   * Revert
   */
  controller.revert();
  QCOMPARE(controller.currentData(1), QVariant("AE"));
  QCOMPARE(getClientNameFromDatabase(1), QVariant("AE"));
}

/*
 * Helpers
 */

void SqlTableViewControllerTest::createSchema(const Schema::ClientAddressSchema & schema)
{
  Sql::Schema::Driver driver(database());

  QVERIFY(driver.isValid());
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(driver.createSchema(schema));
}

bool SqlTableViewControllerTest::deleteClientData()
{
  QSqlQuery query(database());
  return query.exec("DELETE FROM Client_tbl");
}

bool SqlTableViewControllerTest::repopulateClientTable(const Schema::ClientPopulation & tp)
{
  Sql::Schema::Driver driver(database());

  Q_ASSERT(driver.isValid());
  if(!deleteClientData()){
    return false;
  }
  return driver.populateTable(tp);
}

QVariant SqlTableViewControllerTest::getClientNameFromDatabase(int id)
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
  SqlTableViewControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
