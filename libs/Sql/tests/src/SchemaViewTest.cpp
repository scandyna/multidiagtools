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
#include "SchemaViewTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/SelectField.h"
#include "Mdt/Sql/Schema/SelectFieldList.h"
#include "Mdt/Sql/Schema/TableTemplate.h"
#include "Mdt/Sql/Schema/View.h"
#include "Schema/Client_tbl.h"
#include <QSqlDatabase>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

void SchemaViewTest::initTestCase()
{
}

void SchemaViewTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaViewTest::selectFieldTest()
{
  using Mdt::Sql::Schema::SelectField;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldName;

  SelectField SF1("Name");
  QCOMPARE(SF1.fieldName(), QString("Name"));
  QVERIFY(SF1.alias().isEmpty());
  SelectField SF2("Name", "AliasName");
  QCOMPARE(SF2.fieldName(), QString("Name"));
  QCOMPARE(SF2.alias(), QString("AliasName"));

//   /*
//    * Setup fields
//    */
//   Field Name;
//   Name.setName("Name");
// 
//   /*
//    * Create based on a field
//    */
//   SelectField SF1(Name);
//   QCOMPARE(SF1.fieldName(), QString("Name"));
//   QVERIFY(SF1.alias().isEmpty());
//   SelectField SF2(Name, "AliasName");
//   QCOMPARE(SF2.fieldName(), QString("Name"));
//   QCOMPARE(SF2.alias(), QString("AliasName"));
//   /*
//    * Create based on a field name
//    */
//   SelectField SF3(FieldName("SF3"));
//   QCOMPARE(SF3.fieldName(), QString("SF3"));
//   QVERIFY(SF3.alias().isEmpty());
//   SelectField SF4(FieldName("SF4"), "Sf4Alias");
//   QCOMPARE(SF4.fieldName(), QString("SF4"));
//   QCOMPARE(SF4.alias(), QString("Sf4Alias"));
}

void SchemaViewTest::selectFieldListTest()
{
  using Mdt::Sql::Schema::SelectField;
  using Mdt::Sql::Schema::FieldName;

  /*
   * Initial state
   */
  Mdt::Sql::Schema::SelectFieldList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
//   list.append("Client_tbl", SelectField(FieldName("Id_PK")));
  list.append("Client_tbl", SelectField("Id_PK"));
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.selectFieldAt(0).fieldName(), QString("Id_PK"));
  QCOMPARE(list.tableNameAt(0), QString("Client_tbl"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

void SchemaViewTest::viewTableTest()
{
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::TableName;

  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Schema::Client_tbl client;

  /*
   * Create based on a Table
   */
  // No alias
  ViewTable CNR1(Connector_tbl);
  QCOMPARE(CNR1.tableName(), QString("Connector_tbl"));
  QVERIFY(CNR1.alias().isEmpty());
  QCOMPARE(CNR1.aliasOrTableName(), QString("Connector_tbl"));
  // With alias
  ViewTable CNR2(Connector_tbl, "CNR");
  QCOMPARE(CNR2.tableName(), QString("Connector_tbl"));
  QCOMPARE(CNR2.alias(), QString("CNR"));
  QCOMPARE(CNR2.aliasOrTableName(), QString("CNR"));
  /*
   * Create based on a TableTemplate
   */
  // No alias
  ViewTable CLI1(client);
  QCOMPARE(CLI1.tableName(), QString("Client_tbl"));
  QVERIFY(CLI1.alias().isEmpty());
  QCOMPARE(CLI1.aliasOrTableName(), QString("Client_tbl"));
  // With alias
  ViewTable CLI2(client, "CLI");
  QCOMPARE(CLI2.tableName(), QString("Client_tbl"));
  QCOMPARE(CLI2.alias(), QString("CLI"));
  QCOMPARE(CLI2.aliasOrTableName(), QString("CLI"));
  /*
   * Create based on a TableName
   */
  // No alias
  ViewTable CNX1(TableName("Connection_tbl"));
  QCOMPARE(CNX1.tableName(), QString("Connection_tbl"));
  QVERIFY(CNX1.alias().isEmpty());
  QCOMPARE(CNX1.aliasOrTableName(), QString("Connection_tbl"));
  // With alias
  ViewTable CNX2(TableName("Connection_tbl"), "CNX");
  QCOMPARE(CNX2.tableName(), QString("Connection_tbl"));
  QCOMPARE(CNX2.alias(), QString("CNX"));
  QCOMPARE(CNX2.aliasOrTableName(), QString("CNX"));
  /*
   * Check mutable usage
   */
  ViewTable vt;
  QVERIFY(vt.isNull());
  vt.setTableName("TableName");
  QVERIFY(!vt.isNull());
  QCOMPARE(vt.tableName(), QString("TableName"));
  QVERIFY(vt.alias().isEmpty());
  QCOMPARE(vt.aliasOrTableName(), QString("TableName"));
  vt.setAlias("Alias");
  QCOMPARE(vt.aliasOrTableName(), QString("Alias"));
  // Clear
  vt.clear();
  QVERIFY(vt.tableName().isEmpty());
  QVERIFY(vt.alias().isEmpty());
  QVERIFY(vt.isNull());
}

void SchemaViewTest::viewTest()
{
  using Mdt::Sql::Schema::View;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::Schema::SelectField;
  using Mdt::Sql::Schema::TableName;
  using Mdt::Sql::Schema::FieldName;

  Schema::Client_tbl client;
  ViewTable CLI(client, "CLI");
  View view;

  /*
   * Initial state
   */
  QVERIFY(view.selectOperator() == View::Select);
  
  QVERIFY(view.isNull());
  /*
   * Simple (single table) view
   */
  view.clear();
  view.setName("Simple_view");
  view.setSelectOperator(View::SelectDistinct);
  view.setTable(CLI);
  view.addSelectField(CLI, client.Id_PK());
  view.addSelectField(CLI, client.Name(), "ClientName");
  // Check
  QVERIFY(!view.isNull());
  QCOMPARE(view.name(), QString("Simple_view"));
  QVERIFY(view.selectOperator() == View::SelectDistinct);
  QCOMPARE(view.tableName(), QString("Client_tbl"));
  QCOMPARE(view.selectFieldList().size(), 2);
  
  /*
   * Clear
   */
  view.clear();
  QVERIFY(view.name().isEmpty());
  QVERIFY(view.selectOperator() == View::Select);
  QVERIFY(view.tableName().isEmpty());
  QVERIFY(view.selectFieldList().isEmpty());
  
  QVERIFY(view.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaViewTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
