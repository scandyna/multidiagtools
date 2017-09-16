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
#include "SqlTableWidgetMapperControllerTest.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/ItemEditor/SqlTableWidgetMapperController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSqlTableModel>
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;
namespace Sql = Mdt::Sql;
using Schema::Client;
using Schema::Address;
using Schema::ClientAddressSchema;
using Schema::ClientPopulation;

void SqlTableWidgetMapperControllerTest::initTestCase()
{
  if(!initDatabaseSqlite()){
    QSKIP("Could not init database - Skip all tests");  // Will also skip all tests
  }
  createSchema(ClientAddressSchema());
}

void SqlTableWidgetMapperControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

// Bug discovered 20170426 with SQlClientEditor example
void SqlTableWidgetMapperControllerTest::lineEditUpdateOnNewRow()
{
  QVERIFY(deleteClientData());
  QLineEdit editor0;
  /*
   * Setup
   */
  SqlTableWidgetMapperController controller;
  controller.setInsertLocation(SqlTableWidgetMapperController::InsertAtEnd);
  controller.setDefaultModel(database());
  controller.addMapping(&editor0, 0);
  controller.setTable(Client());
  QVERIFY(controller.select());
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Insert first row
   */
  controller.insert();
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(editor0.text().isEmpty());
  editor0.setText("1");
  QVERIFY(controller.submit());
  QCOMPARE(editor0.text(), QString("1"));
  /*
   * Insert a second row
   */
  controller.insert();
  QCOMPARE(controller.currentRow(), 1);
  QVERIFY(editor0.text().isEmpty());
  QVERIFY(controller.submit());
  controller.toLast();
  QVERIFY(!editor0.text().isEmpty());
  QVERIFY(editor0.text() != QString("1"));
}

/*
 * Helpers
 */

void SqlTableWidgetMapperControllerTest::createSchema(const Schema::ClientAddressSchema& schema)
{
  Sql::Schema::Driver driver(database());

  QVERIFY(driver.isValid());
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(driver.createSchema(schema));
}

bool SqlTableWidgetMapperControllerTest::deleteClientData()
{
  QSqlQuery query(database());
  return query.exec("DELETE FROM Client_tbl");
}

bool SqlTableWidgetMapperControllerTest::repopulateClientTable(const Schema::ClientPopulation& tp)
{
  Sql::Schema::Driver driver(database());

  Q_ASSERT(driver.isValid());
  if(!deleteClientData()){
    return false;
  }
  return driver.populateTable(tp);
}

QVariant SqlTableWidgetMapperControllerTest::getClientNameFromDatabase(int id)
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
  SqlTableWidgetMapperControllerTest test;

  return QTest::qExec(&test, argc, argv);
}
